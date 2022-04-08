use signals2::*;

pub struct WSlot {
    status: Connection,
}

impl WSlot {
    fn new(conn: Connection) -> Self {
        Self { status: conn }
    }
    pub fn is_connected(&self) -> bool {
        self.status.connected()
    }
    pub fn disconnect(&self) {
        self.status.disconnect()
    }
}

pub struct WSigSlot {
    s: Signal<()>,
}

impl Clone for WSigSlot {
    fn clone(&self) -> Self {
        Self { s: self.s.clone() }
    }
}

impl Default for WSigSlot {
    fn default() -> Self {
        Self::new()
    }
}

impl WSigSlot {
    #[must_use]
    pub fn new() -> Self {
        Self { s: Signal::new() }
    }

    pub fn connect<F>(&self, p_slot: F) -> WSlot
    where
        F: Fn() + Send + Sync + 'static,
    {
        let c = self.s.connect(p_slot);
        WSlot::new(c)
    }

    pub fn emit(&mut self) {
        self.s.emit();
    }
}
