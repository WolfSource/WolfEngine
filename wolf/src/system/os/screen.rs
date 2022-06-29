use anyhow::{anyhow, Result};
use scrap::{Capturer, Display, Frame};

pub struct Screen {
    pub capturer: Capturer,
    pub height: usize,
    pub width: usize,
}

impl Screen {
    /// # Errors
    ///
    /// TODO:
    pub fn new_primary() -> Result<Self> {
        const TRACE: &str = "screen::new_primary";

        let display = Display::primary().map_err(|e| {
            anyhow!(
                "couldn't find primary display, because {}. trace info {}",
                e,
                TRACE
            )
        })?;

        let width = display.width();
        let height = display.height();
        let capturer = Capturer::new(display).map_err(|e| {
            anyhow!(
                "couldn't capture from display because {}. trace info {}",
                e,
                TRACE
            )
        })?;

        Ok(Self {
            capturer,
            height,
            width,
        })
    }

    /// # Errors
    ///
    /// TODO:
    pub fn new_all() -> Result<Vec<Self>> {
        const TRACE: &str = "screen::new_all";

        let displays = Display::all().map_err(|e| {
            anyhow!(
                "couldn't find primary display because {} trace info {}",
                e,
                TRACE
            )
        })?;

        let mut instances = Vec::new();
        for display in displays {
            let width = display.width();
            let height = display.height();

            let capturer = Capturer::new(display).map_err(|e| {
                anyhow!(
                    "couldn't capture from display. because {} trace info {}",
                    e,
                    TRACE
                )
            })?;

            instances.push(Self {
                capturer,
                height,
                width,
            });
        }
        Ok(instances)
    }

    /// # Errors
    ///
    /// TODO:
    pub fn capture(&mut self) -> std::io::Result<Frame> {
        self.capturer.frame()
    }
}
