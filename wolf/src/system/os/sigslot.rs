use signals2::{Connect0, Connection, Emit0, Signal};

pub struct Slot {
    status: Connection,
}

impl Slot {
    fn new(conn: Connection) -> Self {
        Self { status: conn }
    }
    #[must_use]
    pub fn is_connected(&self) -> bool {
        self.status.connected()
    }
    pub fn disconnect(&self) {
        self.status.disconnect();
    }
}

pub struct SigSlot {
    s: Signal<()>,
}

impl Clone for SigSlot {
    fn clone(&self) -> Self {
        Self { s: self.s.clone() }
    }
}

impl Default for SigSlot {
    fn default() -> Self {
        Self::new()
    }
}

impl SigSlot {
    #[must_use]
    pub fn new() -> Self {
        Self { s: Signal::new() }
    }

    pub fn connect<F>(&self, p_slot: F) -> Slot
    where
        F: Fn() + Send + Sync + 'static,
    {
        let c = self.s.connect(p_slot);
        Slot::new(c)
    }

    pub fn emit(&mut self) {
        self.s.emit();
    }
}
