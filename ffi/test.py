from wolf import * # import all functions from wolf.dll

init_lib("./target/debug/wolf.dll")
print(hex(wolf_version()))