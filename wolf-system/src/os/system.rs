use sysinfo::{ProcessExt, SystemExt};
use tokio::process::{Child, Command};

pub struct System {
    sys: sysinfo::System,
}

impl System {
    pub fn new() -> Self {
        // create sysinfo system object
        let mut sys = sysinfo::System::new_all();

        // update all the information of our system struct.
        sys.refresh_all();

        // return ProcessInfo
        Self { sys }
    }

    pub fn refresh_all(&mut self) {
        // update all the information of our system struct.
        self.sys.refresh_all();
    }

    pub fn refresh_system(&mut self) {
        self.sys.refresh_system();
    }

    pub fn refresh_processes(&mut self) {
        self.sys.refresh_processes();
    }

    pub fn refresh_disks(&mut self) {
        self.sys.refresh_disks();
    }

    pub fn refresh_networks(&mut self) {
        self.sys.refresh_networks();
    }

    pub fn number_of_process_instances(&self, p_process_name: &str) -> usize {
        let mut num: usize = 0;
        for (_pid, proc) in self.sys.processes() {
            if proc.name() == p_process_name {
                num += 1;
            }
        }
        num
    }

    pub fn is_process_running_by_pid(&self, p_process_id: &i32) -> bool {
        for (pid, _proc) in self.sys.processes() {
            if pid == p_process_id {
                return true;
            }
        }
        false
    }

    pub fn is_process_running_by_pname(&self, p_process_name: &str) -> bool {
        for (_pid, proc) in self.sys.processes() {
            if proc.name() == p_process_name {
                return true;
            }
        }
        false
    }

    pub fn used_memory_in_kb(&self) -> u64 {
        self.sys.used_memory()
    }

    pub fn total_memory_in_kb(&self) -> u64 {
        self.sys.total_memory()
    }

    pub fn used_swap_in_kb(&self) -> u64 {
        self.sys.used_swap()
    }

    pub fn total_swap_in_kb(&self) -> u64 {
        self.sys.total_swap()
    }
}

pub async fn create_process(
    p_process_path: &str,
    p_process_args: &[&str],
) -> anyhow::Result<Child> {
    //check is file
    let meta = tokio::fs::metadata(p_process_path).await?;
    if meta.is_file() {
        let child = Command::new(p_process_path).args(p_process_args).spawn()?;
        anyhow::Result::Ok(child)
    } else {
        anyhow::bail!("Process {} is not a valid file", p_process_path)
    }
}

#[tokio::main]
#[test]
async fn tests() -> () {
    let path_to_the_process;

    #[cfg(target_os = "windows")]
    {
        path_to_the_process = "C:/Windows/System32/NotePad.exe";
    }
    #[cfg(target_os = "macos")]
    {
        path_to_the_process = "/System/Applications/TextEdit.app/Contents/MacOS/TextEdit";
    }
    // TODO
    // #[cfg(target_os = "linux")]
    // {
    //     path_to_the_process = "";
    // }

    let sys = System::new();
    println!(
        "Memory: {}KB/{}KB",
        sys.used_memory_in_kb(),
        sys.total_memory_in_kb()
    );
    println!(
        "Swap: {}KB/{}KB",
        sys.used_swap_in_kb(),
        sys.total_swap_in_kb()
    );

    if path_to_the_process.is_empty() {
        println!("process name is empty");
    } else {
        let child_res = create_process(path_to_the_process, &[]).await;
        std::thread::sleep(std::time::Duration::from_secs(5));
        println!("process status is {:?}", child_res);
    }
}
