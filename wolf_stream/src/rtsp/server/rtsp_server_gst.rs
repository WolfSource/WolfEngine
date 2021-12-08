use gstreamer_rtsp::{
    glib::{self, SourceId},
    gst,
};
use gstreamer_rtsp_server::{
    prelude::RTSPServerExtManual,
    traits::{RTSPMediaFactoryExt, RTSPMountPointsExt, RTSPServerExt},
    RTSPMediaFactory, RTSPServer, RTSPTransportMode,
};
use std::sync::Once;

static ONCE_INIT: Once = Once::new();
static ONCE_FINI: Once = Once::new();

#[derive(Debug, Copy, Clone)]
pub enum RtspTransportProtocol {
    Tcp = 0,
    Udp,
}

#[derive(Debug)]
pub struct RtspServerGst {
    server: RTSPServer,
    port: u16,
    path: String,
    gst_cmd: String,
    transport_protocol: RtspTransportProtocol,
    multicast: bool,
    main_loop: glib::MainLoop,
    source_id: Option<SourceId>,
}

impl Default for RtspServerGst {
    fn default() -> Self {
        Self::new()
    }
}

impl Clone for RtspServerGst {
    fn clone(&self) -> Self {
        Self {
            server: self.server.clone(),
            port: self.port,
            path: self.path.clone(),
            gst_cmd: self.gst_cmd.clone(),
            transport_protocol: self.transport_protocol,
            multicast: self.multicast,
            main_loop: self.main_loop.clone(),
            source_id: None,
        }
    }
    fn clone_from(&mut self, source: &Self) {
        self.server = source.server.clone();
        self.port = source.port;
        self.path = source.path.clone();
        self.gst_cmd = source.gst_cmd.clone();
        self.transport_protocol = source.transport_protocol;
        self.multicast = source.multicast;
        self.main_loop = source.main_loop.clone();
    }
}

impl RtspServerGst {
    /// Initialize gstreamer rtsp server. This function should be called once for a time
    /// before calling other functions
    pub fn init() {
        ONCE_INIT.call_once(|| {
            let _r = gst::init().map_err(|e| {
                anyhow::anyhow!(
                    "could not initialize gstreamer because {:?}. Trace: rtsp_gst::init",
                    e,
                )
            });
        });
    }

    /// Create an instance from rtsp server.
    #[must_use]
    pub fn new() -> Self {
        Self {
            server: RTSPServer::new(),
            port: 554,
            path: String::new(),
            gst_cmd: String::new(),
            transport_protocol: RtspTransportProtocol::Tcp,
            multicast: true,
            main_loop: glib::MainLoop::new(None, false),
            source_id: None,
        }
    }

    /// Initialize rtsp server. This function won't run rtsp stream and
    /// you need to call start function after this function
    /// # Errors
    ///
    /// Will return `Err` if `rtsp` could not mount or attach.
    pub fn initialize(
        &mut self,
        p_port: u16,
        p_path: &str,
        p_transport_protocol: RtspTransportProtocol,
        p_multicast: bool,
        p_cmd: &str,
    ) -> anyhow::Result<()> {
        self.port = p_port;
        self.path = p_path.to_string();
        self.transport_protocol = p_transport_protocol;
        self.multicast = p_multicast;
        self.gst_cmd = p_cmd.to_string();

        self.server.set_service(&self.port.to_string());

        // Much like HTTP servers, RTSP servers have multiple endpoints that
        // provide different streams. Here, we ask our server to give
        // us a reference to his list of endpoints, so we can add our
        // endpoint, providing the pipeline from the cli.
        let mounts_res = RTSPServerExt::mount_points(&self.server);
        let ret = match mounts_res {
            Some(mp) => {
                // Next, we create a factory for the endpoint we want to create.
                // The job of the factory is to create a new pipeline for each client that
                // connects, or (if configured to do so) to reuse an existing pipeline.
                let factory = RTSPMediaFactory::new();

                // Here we tell the media factory the media we want to serve.
                // This is done in the launch syntax. When the first client connects,
                // the factory will use this syntax to create a new pipeline instance.
                factory.set_launch(&self.gst_cmd);

                // This setting specifies whether each connecting client gets the output
                // of a new instance of the pipeline, or whether all connected clients share
                // the output of the same pipeline.
                // If you want to stream a fixed video you have stored on the server to any
                // client, you would not set this to shared here (since every client wants
                // to start at the beginning of the video). But if you want to distribute
                // a live source, you will probably want to set this to shared, to save
                // computing and memory capacity on the server.
                factory.set_shared(self.multicast);

                // set transport mode
                factory.set_transport_mode(RTSPTransportMode::PLAY);

                // set protocol
                match self.transport_protocol {
                    RtspTransportProtocol::Tcp => {
                        factory.set_protocols(gstreamer_rtsp::RTSPLowerTrans::TCP);
                    }
                    RtspTransportProtocol::Udp => {
                        factory.set_protocols(gstreamer_rtsp::RTSPLowerTrans::UDP);
                    }
                };

                // Now we add a new mount-point and tell the RTSP server to serve the content
                // provided by the factory we configured above, when a client connects to
                // this specific path.
                mp.add_factory(&self.path, &factory);

                // Attach the server to our main context.
                // A main context is the thing where other stuff is registering itself for its
                // events (e.g. sockets, GStreamer bus, ...) and the main loop is something that
                // polls the main context for its events and dispatches them to whoever is
                // interested in them. In this example, we only do have one, so we can
                // leave the context parameter empty, it will automatically select
                // the default one.
                let attach_res = self.server.attach(None);
                let ret = match attach_res {
                    Ok(id) => {
                        self.source_id = Some(id);
                        Ok(())
                    }
                    Err(e) => {
                        anyhow::bail!(
                            "could not attach to our main context because {:?}. trace: rtsp_gst::start", e
                        )
                    }
                };
                ret
            }
            None => {
                anyhow::bail!("could not create RTSP mount points. trace: rtsp_gst::start")
            }
        };
        ret
    }

    /// Start the mainloop. From this point on, the server will start to serve
    /// our quality content to connecting clients.
    pub fn start(&self) {
        self.main_loop.run();
    }

    /// Stop the mainloop and remove the source id
    pub fn stop(&mut self) {
        if self.main_loop.is_running() {
            self.main_loop.quit();
        }
        if self.source_id.is_some() {
            if let Some(id) = self.source_id.take() {
                glib::source_remove(id);
            };
        }
    }

    #[must_use]
    pub const fn get_port(&self) -> u16 {
        self.port
    }

    #[must_use]
    pub fn get_path(&self) -> String {
        self.path.clone()
    }

    #[must_use]
    pub fn get_gst_cmd(&self) -> String {
        self.gst_cmd.clone()
    }

    #[must_use]
    pub const fn get_transport_protocol(&self) -> RtspTransportProtocol {
        self.transport_protocol
    }

    #[must_use]
    pub const fn get_multicast(&self) -> bool {
        self.multicast
    }

    /// Finitialize gstreamer rtsp server. This function should be called at the end of program
    pub fn fini() {
        ONCE_FINI.call_once(|| {
            unsafe { gst::deinit() };
        });
    }
}

#[tokio::main]
#[test]
async fn tests() {
    RtspServerGst::init();

    let mut rtsp = RtspServerGst::new();
    let mut cloned = rtsp.clone();

    let _r = std::thread::spawn(move || {
        const SECS: u64 = 20;
        println!("rtsp stream will be shutdown after {} seconds", SECS);
        std::thread::sleep(std::time::Duration::from_secs(SECS));
        cloned.stop();
    });

    let ret = rtsp.initialize(554, "/play", RtspTransportProtocol::Tcp , true, "videotestsrc ! videoconvert ! videoscale ! video/x-raw,width=640,height=360,framerate=60/1 ! x264enc ! video/x-h264,width=640,height=360,framerate=60/1,profile=(string)high ! rtph264pay name=pay0 pt=96")
    .map(|_|
    {
        println!(
            "stream is ready at rtsp://127.0.0.1:{}{}",
            rtsp.get_port(),
            rtsp.get_path()
        );
        rtsp.start();
    }).map_err(|e|
    {
        anyhow::anyhow!("{:?}", e)
    });

    RtspServerGst::fini();
    assert!(ret.is_ok(), "{:?}", ret);
}
