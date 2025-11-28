```markdown
Tetris build instructions (GitHub Actions)
=========================================

This repository contains tetris.cpp and a GitHub Actions workflow that builds a Windows executable (tetris.exe)
using mingw-w64 on an Ubuntu runner and uploads it as an artifact.

How to generate tetris.exe from a phone/tablet
1. Create a new GitHub repository (use the GitHub app or github.com in your mobile browser).
2. Add the files:
   - Add a new file called `tetris.cpp` and paste the C++ source (from this repo).
   - Add a new file `.github/workflows/build.yml` and paste the workflow YAML.
   - Add `README.md` (optional).
   Use "Add file → Create new file" in the GitHub web UI and Commit each file to the `main` branch.
3. Trigger the workflow:
   - After committing, go to the repo's "Actions" tab.
   - Open the "Build Tetris (Windows EXE)" workflow.
   - Press "Run workflow" (choose `main`) OR just wait — the workflow also runs on push to main automatically.
4. Wait for the workflow to finish:
   - The job will compile tetris.cpp into tetris.exe and upload it as an artifact named `tetris-exe`.
5. Download the artifact:
   - In the workflow run page click the successful run, then tap "Artifacts" and download `tetris-exe`.
   - The file downloads to your phone's Downloads folder; you can transfer it to a Windows machine or open with a file manager.

Notes
- The build uses mingw-w64 to produce a Windows executable.
- If the workflow fails, open the run logs (Actions → run → job → steps) and copy the error text; I can help interpret it.
- If you want both 32-bit and 64-bit builds or a zipped artifact, tell me and I'll update the workflow.
```
