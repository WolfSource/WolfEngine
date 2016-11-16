Wolf Engine
===========
<img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/master/Logo.jpg" width="256" height="256" alt="WolfSource"/>
<p><b>Welcome to the Wolf Engine source code.</b></p> 
<p>The&nbsp;<b>Wolf Engine</b>&nbsp;is the next
generation of&nbsp;<a href="https://persianengine.codeplex.com/">Persian Game Engine</a>&nbsp;which is a
cross-platform open source game engine created by&nbsp;<a href="http://pooyaeimandar.com/">Pooya Eimandar</a>.
Wolf Engine written in C++ and support Python as an embedded script language.
The Wolf used DirectX for rendering on <b>Windows Win32 and Universal Windows Platform</b>, however I’m working on <b>Vulkan render engine for Android/Ubuntu</b>, <b>Apple Metal render engine for OSX/IOS</b> and <ab>webGL for HTML5 platform</b></p>

<!--What's new-->
<h2><a id="user-content-whatsnew" class="anchor" href="#whatsnew" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>What’s new on version 0.5.0.0</h2>
<h3>New Features</h3>

<ul>
<li>"<b>w_line_shape</b>" has been added to "<b>GUI framework</b>" of DirectX 11.X</li>
<li>"<b>w_rounded_rectangle_shape</b>" has been added to "<b>GUI framework</b>" of DirectX 11.X</li>
<li>"<b>w_ellipse_shape</b>" has been added to "<b>GUI framework</b>" of DirectX 11.X</li>
<li>"<b>w_list_widget</b>" has been added to "<b>GUI framework</b>" of DirectX 11.X with support of both wolf::gui and gdi controls</li>
<li>Reducing <b>texture</b> memory usage by 2 channel encoding</li>
<li>New samples have been added. Check out:
  <ul>
    <li>Samples\03-Direct2D\03_Animated_Geometries_DX11_X.Win32</li>
    <li>Samples\05-Buffers\01_Quad_DX11_X.Win32</li>
    <li>Samples\05-Buffers\02_Textured_Quad.DX11_X.Win32</li>
  </ul>
</li>
<li>"<b>Wolf.System</b>" has been ported to "<b>Android Nvidia Tegra Shield</b>"</li>
<li>"<b>Wolf.System</b>" has been ported to "<b>Windows Universal</b>"</li>
</ul>

<h3>Improved</h3>
<ul>
  <li>Drawing dynamic vertex buffers of gui improved</li>
  <li>wolf::graphics::w_mesh is independent drawing class, which can use for drawing any geometry with any texture and effect</li>
  <li>wolf::gui::w_image is render with w_texture_2d instead of slow direct2D image rendering system</li>
  <li>ffmpeg allocating dynamic buffer for video and audio optimized in this version</li>
  <li>Classes "<b>w_mesh and w_quad</b>" optimized. <b>w_quad<T></b> is a template class available for various constant buffers</li>
  <li>Support multiple image formats with stb_image. Now wolf::graphics::w_texture::load_texture_2D_from_file function supports following formats:
  <ul>
    <li>dds</li>
    <li>png</li>
    <li>jpg</li>
    <li>tga</li>
    <li>psd</li>
  </ul>
</li>
</ul>

<h3>Bug Fixes</h3>
<ul>
<li>Memory leaks for <b>wolf::gui::w_label</b> fixed</li>
</ul>

<!--Projects-->
<h2><a id="user-content-projects" class="anchor" href="#projects" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Projects using Wolf</h2>
<ul>
<li>&quot;<a href="https://www.youtube.com/watch?v=EZSdEjBvuGY"><b>Wolf.Playout</b></a>&quot; is a playout automation software which is used by <a href="http://en.alalam.ir/">Alalam news
network TV</a></li>
<li>&quot;<b>Wolf.CG</b>&quot; is a true real time 3D graphics and character generator which can be integrate
with any newsroom using python API.</li>
</ul>
<p>If you are using Wolf Engine in your projects, kindly let us know about your project.</p>

<!--Branches-->
<h2><a id="user-content-branches" class="anchor" href="#branches" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Branches</h2>
<p>From this repository you can build the Wolf
Engine for Windows, modify them in any way you can imagine, and share your
changes with others. Wolf Engine publishes source in two rolling branches:</p>
<ul>
<li>The <a href="https://github.com/PooyaEimandar/Wolf.Engine/releases">release branch</a>&nbsp;is
extensively tested and makes a great starting point for learning how to use Wolf Engine or making your own real time application or games.</li>
<li>The&nbsp;<a href="https://github.com/PooyaEimandar/Wolf.Engine/tree/master">master branch</a> which is the original source and may be buggy.</li>
</ul>

<!--Building-->
<h2><a id="user-content-building" class="anchor" href="#building" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Building & Running</h2>
<p>
To help you navigate the various ways you can use Wolf Engine, I have assembled a collection of learning tutorials for you at <a href="http://wolfsource.io/gpunotes/08082016"><b>GPU Notes blog</b></a>.
</p>

<!--Samples-->
<h2><a id="user-content-samples" class="anchor" href="#samples" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Samples</h2>
<p>
Here are the screenshots of samples, you can read full information about these samples on GPU Notes, just click on the name of sample and you will be redirected to the specific page.
</p>

<!-- Screenshots -->
<h2>01-Initialize</h2>
<table>
  <tr>
    <td>
      <img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/0.5.0.0/samples/windows/01-Initialize/01-window/view.jpg" alt="01_window/view.jpg" width="256" height="192"/>
      <p><b>Win32</b>: 01-window</p>
    </td>
    <td>
      <img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/0.5.0.0/samples/windows/01-Initialize/02-multiple_windows/view.jpg" alt="02_multiple_windows/view.jpg" width="256" height="192"/>
      <p><b>Win32</b>: 02-multiple_windows</p>
    </td>
    <td>
      <img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/0.5.0.0/samples/windows/01-Initialize/03-scene/view.jpg" alt="03_scene/view.jpg" width="256" height="192"/>
      <p><b>Win32</b>: 03-scene</p>
    </td>
  </tr>
  <tr>
    <td>
      <img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/0.5.0.0/samples/windows/01-Initialize/04-one_scene_multiple_windows/view.jpg" alt="04_one_scene_multiple_windows/view.jpg" width="256" height="192"/>
      <p><b>Win32</b>: 04-one_scene_multiple_windows</p>
    </td>
    <td>
      <img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/0.5.0.0/samples/windows/01-Initialize/05-multiple_scenes_multiple_windows/view.jpg" alt="05_multiple_scenes_multiple_windows/view.jpg" width="256" height="192"/>
      <p><b>Win32</b>: 05-multiple_scenes_multiple_windows</p>
    </td>
  </tr>
</table>

<h2>02-GUI</h2>
<table>
  <tr>
    <td>
      <img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/0.5.0.0/samples/windows/02-GUI/01-widget/view.jpg" alt="01-widget/view.jpg" width="256" height="192"/>
      <p><b>Win32</b>: 01-widget</p>
    </td>
    <td>
      <img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/0.5.0.0/samples/windows/02-GUI/02-gui%20from%20xml/view.jpg" alt="02-gui from xml/view.jpg" width="256" height="192"/>
      <p><b>Win32</b>: 02-gui from xml</p>
    </td>
  </tr>
</table>

<h2>03-Direct2D</h2>
<table>
  <tr>
    <td>
      <img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/0.5.0.0/samples/windows/03-Direct2D/01-shapes/view.jpg" alt="01-shapes/view.jpg" width="256" height="192"/>
      <p><b>Win32</b>: 01-shapes</p>
    </td>
    <td>
      <img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/0.5.0.0/samples/windows/03-Direct2D/02-geometries/view.jpg" alt="02-geometries/view.jpg" width="256" height="192"/>
      <p><b>Win32</b>: 02-geometries</p>
    </td>
    <td>
      <img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/0.5.0.0/samples/windows/03-Direct2D/03-animated_geometries/view.jpg" alt="02-geometries/view.jpg" width="256" height="192"/>
      <p><b>Win32</b>: 03-animated_geometries</p>
    </td>
  </tr>
</table>

<h2>05-Buffers</h2>
<table>
  <tr>
    <td>
      <img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/0.5.0.0/samples/windows/05-Buffers/01-quad/view.jpg" alt="01-shapes/view.jpg" width="256" height="192"/>
      <p><b>Win32</b>: 01-quad</p>
    </td>
    <td>
      <img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/0.5.0.0/samples/windows/05-Buffers/02-textured_quad/view.jpg" alt="02-geometries/view.jpg" width="256" height="192"/>
      <p><b>Win32</b>: 02-textured_quad</p>
    </td>
  </tr>
</table>

<!--Licensing-->
<h2><a id="user-content-licensing" class="anchor" href="#licensing" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Licensing</h2>
<p>
Your access to and use of Wolf Engine on GitHub is governed by the&nbsp;<a href="https://github.com/PooyaEimandar/Wolf.Engine/blob/master/LICENSE">License Agreement</a>. If you don't agree to those terms, then you are not permitted to access or use Wolf Engine.
</p>

<!--Issues-->
<h2><a id="user-content-issues" class="anchor" href="#issues" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Issues</h2>
<p>
Kindly report any issues or post your question
about Wolf Engine on <a href="https://github.com/PooyaEimandar/Wolf.Engine/issues">Issues page</a>. If you have any questions about samples or tutorials, kindly comments on related sample/tutorial page on <a href="http://wolfsource.io/gpunotes/wolfengine">wolfsource.io</a>
</p>

<!--Additional Notes-->
<h2><a id="user-content-notes" class="anchor" href="#notes" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Additional Notes</h2>
<p>
I provided a dedicated blog for gpu programming, make sure check it out at <a href="http://wolfsource.io/gpunotes/">WolfSource.io/GPUNotes</a>.
</p>
<p>
Additionally, in case you are interested, I've written a book with the focus on DirectX 11.1 Game
Programming, you can order it from <a href="http://www.packtpub.com/directx-11-1-game-programming/book">PackT Publishing</a>.
</p>

<!--Road map on Trello-->
<h2><a id="user-content-trello" class="anchor" href="#trello" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Road map</h2>
<a href="https://trello.com/b/O8axM8lj/wolf-engine">Trello</a>

<!--Twitter-->
<h2><a id="user-content-twitter" class="anchor" href="#twitter" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Twitter</h2>
<a href="https://twitter.com/WolfSource_io">@WolfSource_io</a>
