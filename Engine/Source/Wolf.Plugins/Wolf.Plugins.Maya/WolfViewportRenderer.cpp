#include "pch.h"
#include "WolfViewportRenderer.h"

using namespace std;

const unsigned int bytesPerPixel = 4;
static const UINT Width = 800;
static const UINT Height = 600;

// Dummy window callback procedure.
LRESULT CALLBACK D3DWindowProc(HWND pHWnd, UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
	LRESULT lResult = 0;
	switch (pMsg)
	{
	default:
		lResult = DefWindowProc(pHWnd, pMsg, pWParam, pLParam);
		break;
	case WM_CLOSE:
		//PostQuitMessage(0);	-- can't allow this. Will kill Maya
		break;

	case WM_DESTROY:
		//PostQuitMessage(0);	-- can't allow this. Will kill Maya
		break;
	}
	return lResult;
}

WolfViewportRenderer::WolfViewportRenderer() : MViewportRenderer("WolfViewportRenderer")
{
	// Set the ui name
	fUIName.set("Wolf Engine");

	// Overrides all drawing but allows the HUD to be drawn on top
	this->fRenderingOverride = MViewportRenderer::kOverrideThenUI;

	// Set API and version number
	this->mayaAPI = MViewportRenderer::kDirect3D;
	this->mayaVersion = 11.1f;
}

WolfViewportRenderer::~WolfViewportRenderer()
{
	uninitialize();
}

MStatus WolfViewportRenderer::initialize()
{
	auto status = MStatus::kSuccess;

	// Create the window to contain our off-screen target.
	if (!this->mayaHwnd)
	{
		wstring winClassName;
		//Try registering class with random name for three times
		bool doneRegistering = false;
		for (int i = 0; i < 3; ++i)
		{
			winClassName = L"WolfEngineCLS" + std::to_wstring(std::rand());
			// Register the window class
			winClass =
			{
				sizeof(WNDCLASSEX),
				CS_CLASSDC,
				(WNDPROC) D3DWindowProc,
				0L,
				0L,
				GetModuleHandle(NULL),
				NULL,
				NULL,
				NULL,
				NULL,
				winClassName.c_str(),
				NULL
			};

			if (RegisterClassEx(&this->winClass))
			{
				this->mayaHwnd = CreateWindow(
					this->winClass.lpszClassName,
					L"Wolf Engine",
					WS_OVERLAPPEDWINDOW,
					0,
					0,
					Width,
					Height,
					NULL,
					NULL,
					this->winClass.hInstance,
					NULL);

				if (!this->mayaHwnd)
				{
					LogError("Could not create hwnd for off screen window of Maya");
					status = MStatus::kFailure;
					CHECK_MSTATUS_AND_RETURN_IT(status);
					return status;
				}
				doneRegistering = true;
			}
			else
			{
				LogError("Could not register window class");
				auto msg = std::to_string(i) + ((i == 1 || i == 2) ? "nd" : "th") + " try for creating another window class";
				Log(msg);
			}
		}

		if (!doneRegistering)
		{
			status = MStatus::kFailure;
			CHECK_MSTATUS_AND_RETURN_IT(status);
			return status;
		}
	}
	
	this->mayaBackBuffer.create(Width, Height, bytesPerPixel/* MPixelType type = kByte */);
	this->mayaBackBuffer.setRGBA(false);
	
	// Create an appropriate device
	if (this->mayaHwnd)
	{
		this->game = std::make_unique<Game>();
		this->game->Initialize(this->mayaHwnd, Width, Height);
		if (this->renderPixels == nullptr)
		{
			this->renderPixels = new (std::nothrow) BYTE[Width * Height * bytesPerPixel];//4 bytes
			if (!this->renderPixels) { OnFailed(S_FALSE, "E_OUTOFMEMORY on creating pixels", "WolfViewportRenderer", true); }
		}
		LoadMayaScene();
	}
	
	return status;
}

void WolfViewportRenderer::CheckCommands()
{
	using namespace Wolf;

	if (Wolf::Commands == WCommnads::NOP) return;
	auto commandID = (BYTE) Wolf::Commands;
	switch (Wolf::Commands)
	{
	case WCommnads::LoadScene:
		LoadMayaScene();
		break;
	}

	//Reset the command
	Wolf::Commands = WCommnads::NOP;
}

void WolfViewportRenderer::LoadMayaScene()
{
	//Read all meshes
	MItDependencyNodes it(MFn::kMesh);

	bool problem = false;
	while (!it.isDone())
	{
		// get a handle to this node
		MObject obj = it.item();
		// write the node type found
		//Log(obj.apiTypeStr());

		auto type = obj.apiType();
		if (type == MFn::Type::kMesh)
		{
			using namespace Wolf::Graphics;

			auto mesh = new MFnMesh(obj);
			auto meshName = std::string(mesh->name().asChar());

			if (!this->game->IsMeshExistInCurrentScene(meshName))
			{
				Log(meshName + " is not exist in scene manager");

				vector<float> vertices;
				vector<UINT16> indices;
				//Read vertices
				auto pointVertices = new MPointArray();
				{
					mesh->getPoints(*pointVertices);

					auto vLenght = pointVertices[0].length();
					for (size_t i = 0; i < vLenght; ++i)
					{
						vertices.push_back(pointVertices[0][i].x);
						vertices.push_back(pointVertices[0][i].y);
						vertices.push_back(pointVertices[0][i].z);
					}

					pointVertices->clear();
					pointVertices = nullptr;
				}

				//Read indices
				auto vc = new MIntArray();
				auto mIndices = new MIntArray();
				mesh->getVertices(*vc, *mIndices);
				auto iLenght = mIndices->length();
				auto _indices = new int[iLenght];
				mIndices->get(_indices);
				for (size_t i = 0; i < iLenght; ++i)
				{
					indices.push_back(_indices[i]);
				}

				mIndices->clear();
				mIndices = nullptr;
				vc->clear();
				vc = nullptr;
				DELETE_ARRAY(_indices);

				//Add to the vector of primitives
				if (!this->game->LoadMesh(meshName, vertices, indices))
				{
					problem = true;
					break;
				}
			}
			else
			{
				Log(meshName + " is exists in scene manager");
			}

			SAFE_DELETE(mesh);
		}

		// move on to the next node
		it.next();
	}

	if (problem)
	{
		Log("Failed to load scene");
	}
}

MStatus WolfViewportRenderer::render(const MRenderingInfo& renderInfo)
{
#pragma region check for resizing happend

	UINT mayaBackBufferWidth = 0;
	UINT mayaBackBufferHeight = 0;
	this->mayaBackBuffer.getSize(mayaBackBufferWidth, mayaBackBufferHeight);

	if (!mayaBackBufferWidth || !mayaBackBufferHeight || mayaBackBufferWidth != Width || mayaBackBufferHeight != Height)
	{
			//Try to resize
			this->mayaBackBuffer.resize(Width, Height, false);
			//Check it again
			this->mayaBackBuffer.getSize(mayaBackBufferWidth, mayaBackBufferHeight);
			if (mayaBackBufferWidth != Width || mayaBackBufferHeight != Height)
			{
				LogError("Could not resize Maya image buffer for reading back!");
				auto status = MStatus::kFailure;
				CHECK_MSTATUS_AND_RETURN_IT(status);
				return status;
			}
	}

#pragma endregion

	CheckCommands();

	if(!this->game->Run(this->renderPixels))
	{
		LogError("Error just happened in rendering wolf");
		auto status = MStatus::kFailure;
		CHECK_MSTATUS_AND_RETURN_IT(status);
		return status;
	}

	this->mayaBackBuffer.setPixels(this->renderPixels, Width, Height);

	MViewportRenderer::RenderingAPI targetAPI = renderInfo.renderingAPI();
	if (targetAPI == MViewportRenderer::kOpenGL)
	{
		const MRenderTarget& target = renderInfo.renderTarget();
		MStatus writeState = target.writeColorBuffer(this->mayaBackBuffer, 0, 0);
		if (writeState != MStatus::kSuccess)
		{
			LogError("Error just happened while writing to Maya color buffer");
			CHECK_MSTATUS_AND_RETURN_IT(writeState);
			return writeState;
		}
	}
	else
	{
		LogError("Unsupported renderer, Wolf Engine supports for only kOpenGL");
		auto status = MStatus::kFailure;
		CHECK_MSTATUS_AND_RETURN_IT(status);
		return status;
	}
	
	return MStatus::kSuccess;
}

MStatus WolfViewportRenderer::uninitialize()
{
	SMART_RELEASE(this->game);
	DELETE_ARRAY(this->renderPixels)
	mayaBackBuffer.release();

	if (UnregisterClass(this->winClass.lpszClassName, this->winClass.hInstance))
	{
		Log("Window class UnRegistered");
	}
	this->mayaHwnd = NULL;
	return MStatus::kSuccess;
}

unsigned int WolfViewportRenderer::overrideThenStandardExclusion() const
{
	// exclude everything apart from manipulators
	return ~(unsigned int) kExcludeManipulators;
}

bool WolfViewportRenderer::nativelySupports(MViewportRenderer::RenderingAPI api, float version)
{
	// Do API and version check
	return ((api == mayaAPI) && (version == mayaVersion));
}

bool WolfViewportRenderer::override(MViewportRenderer::RenderingOverride override)
{
	// Check override
	return (override == fRenderingOverride);
}
