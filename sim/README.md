# NPWT UI Simulator (WSL2)

Runs the same `app/gui_mgr.c` LVGL UI code as the STM32 target inside an SDL2 window on WSL2 Ubuntu at the hardware display resolution (`320x240`).

---

## 1. One-Time WSL2 Setup

Install build tools and SDL2 inside your WSL2 Ubuntu terminal:

```bash
sudo apt update
sudo apt install -y cmake gcc pkg-config libsdl2-dev
```

Verify SDL2 and display support:

```bash
pkg-config --modversion sdl2   # should print 2.x
echo $DISPLAY                  # should print :0 (WSLg on Windows 11)
```

> **Windows 10 only:** Install an X server (e.g. VcXsrv) on Windows and run `export DISPLAY=:0` in WSL2.

---

## 2. Build the Simulator

From WSL2, navigate to the Windows project's `sim/` directory:

```bash
cd /mnt/d/STMWorkspcae_2.3.0/npwt_single_cubemx2/npwt_single_snsr_cmake/sim
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

---

## 3. Run & Test

```bash
./npwt_sim
```

An SDL2 window (`320x240`) will open showing the initial UI screen (`UI_DO_SCREEN`).

### Controls
| PC Key | Hardware Switch |
|---|---|
| `Up Arrow` | `Board_ReadSwUP` |
| `Down Arrow` | `Board_ReadSwDOWN` |
| `Left Arrow` | `Board_ReadSwLEFT` |
| `Right Arrow` | `Board_ReadSwRIGHT` |
| `Enter` | `Board_ReadSwENTER` |
| Mouse | LVGL pointer / click |

### What to Expect Currently
- `sim/main.c` initializes `GuiMgrInit()`, waits for `GuiMgrIsReady()`, and displays `UI_DO_SCREEN` (`DO`, `PH`, `SALINITY`).
- `sim/mock/mock_data.c` runs a background task (`MockDataStart()`) ready to call `GuiMgrUpdateData()` once `GuiMgrUpdateData()` is uncommented in `app/gui_mgr.c`.
- Press `Ctrl+C` in the terminal or close the SDL2 window to exit.

---

## 4. Rebuilding After UI Edits

Whenever you edit `app/gui_mgr.c` or `sim/` files, just re-run `make` inside `sim/build`:

```bash
cd /mnt/d/STMWorkspcae_2.3.0/npwt_single_cubemx2/npwt_single_snsr_cmake/sim/build
make -j$(nproc) && ./npwt_sim
```
