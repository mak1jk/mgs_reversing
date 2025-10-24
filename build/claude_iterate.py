#!/usr/bin/env python3
"""
Claude Code Iteration Helper
Automates the build→verify→analyze cycle for decompilation work.
"""

import sys
import os
import subprocess
import argparse
import time
import hashlib
from pathlib import Path
from colorama import init as colorama_init
from termcolor import colored

colorama_init()

# Import existing project modules
try:
    from compare import TARGET_HASH, TARGET_VR_HASH, TARGET_OVERLAYS_HASH
except ImportError:
    print(colored("ERROR: Could not import compare.py", "red"))
    sys.exit(1)


class BuildVerifier:
    def __init__(self, variant="main_exe", verbose=False, watch_file=None):
        self.variant = variant
        self.verbose = verbose
        self.watch_file = watch_file
        self.build_dir = Path(__file__).parent
        self.obj_dir = self.build_dir / "obj"

    def print_header(self, text):
        """Print a formatted header"""
        print("\n" + "=" * 70)
        print(colored(text.center(70), "cyan", attrs=["bold"]))
        print("=" * 70)

    def print_section(self, text):
        """Print a section header"""
        print("\n" + colored(f">>> {text}", "yellow", attrs=["bold"]))

    def run_build(self):
        """Execute the build process"""
        self.print_section("Running build...")

        cmd = [sys.executable, "build.py"]
        if self.variant != "main_exe":
            cmd.append(f"--variant={self.variant}")

        start_time = time.time()

        try:
            result = subprocess.run(
                cmd,
                cwd=self.build_dir,
                capture_output=True,
                text=True
            )

            build_time = time.time() - start_time

            if result.returncode != 0:
                print(colored(f"✗ Build FAILED in {build_time:.2f}s", "red", attrs=["bold"]))
                print("\nStderr:")
                print(result.stderr)
                return False
            else:
                print(colored(f"✓ Build completed in {build_time:.2f}s", "green", attrs=["bold"]))

            if self.verbose:
                print("\nBuild output:")
                print(result.stdout)

            return True

        except Exception as e:
            print(colored(f"✗ Build exception: {e}", "red"))
            return False

    def verify_hash(self, file_path, expected_hash):
        """Verify SHA256 hash of a file"""
        if not os.path.exists(file_path):
            return False, "File not found"

        sha256 = hashlib.sha256()
        with open(file_path, 'rb') as f:
            sha256.update(f.read())
        actual_hash = sha256.hexdigest()

        if actual_hash == expected_hash:
            return True, "Match"
        else:
            return False, f"Hash mismatch:\n  Expected: {expected_hash}\n  Got:      {actual_hash}"

    def run_verification(self):
        """Verify the build output"""
        self.print_section("Verifying build output...")

        if self.variant == "dev_exe":
            print(colored("ℹ Dev build - skipping verification", "yellow"))
            return True

        # Determine which executable to check
        if self.variant == "vr_exe":
            exe_path = self.obj_dir / "_mgsi_vr.exe"
            target_hash = TARGET_VR_HASH
        else:
            exe_path = self.obj_dir / "_mgsi.exe"
            target_hash = TARGET_HASH

        # Verify main executable
        match, msg = self.verify_hash(exe_path, target_hash)

        if match:
            print(colored(f"✓ Main executable: MATCH", "green", attrs=["bold"]))
        else:
            print(colored(f"✗ Main executable: MISMATCH", "red", attrs=["bold"]))
            if self.verbose:
                print(f"  {msg}")

        # Verify overlays
        overlay_mismatches = []
        overlay_matches = 0

        for overlay_name, overlay_hash in TARGET_OVERLAYS_HASH.items():
            overlay_path = self.obj_dir / f"{overlay_name}.bin"
            overlay_match, overlay_msg = self.verify_hash(overlay_path, overlay_hash)

            if overlay_match:
                overlay_matches += 1
            else:
                overlay_mismatches.append((overlay_name, overlay_msg))

        total_overlays = len(TARGET_OVERLAYS_HASH)

        if overlay_mismatches:
            print(colored(
                f"✗ Overlays: {overlay_matches}/{total_overlays} match",
                "yellow"
            ))
            if self.verbose:
                for name, msg in overlay_mismatches[:5]:  # Show first 5
                    print(f"  ✗ {name}: {msg}")
                if len(overlay_mismatches) > 5:
                    print(f"  ... and {len(overlay_mismatches) - 5} more")
        else:
            print(colored(
                f"✓ Overlays: {overlay_matches}/{total_overlays} match",
                "green",
                attrs=["bold"]
            ))

        return match and not overlay_mismatches

    def analyze_differences(self):
        """Run compare.py to show detailed differences"""
        self.print_section("Analyzing differences...")

        try:
            # Check if capstone is available for detailed disassembly
            import capstone
            has_capstone = True
        except ImportError:
            has_capstone = False
            print(colored("ℹ Install capstone for detailed disassembly diffs", "yellow"))

        # Run compare.py
        result = subprocess.run(
            [sys.executable, "compare.py"],
            cwd=self.build_dir,
            capture_output=True,
            text=True
        )

        print(result.stdout)
        if result.stderr:
            print(result.stderr)

    def print_summary(self, success):
        """Print final summary"""
        self.print_header("SUMMARY")

        if success:
            print(colored("\n🎉 BUILD SUCCESSFUL - PERFECT MATCH!", "green", attrs=["bold"]))
            print("\nThe compiled binary matches the target perfectly.")
            print("This code is ready to commit!")
        else:
            print(colored("\n⚠ BUILD COMPLETED - NEEDS ITERATION", "yellow", attrs=["bold"]))
            print("\nThe compiled binary does not match the target.")
            print("Review the differences above and iterate on the source code.")

        if self.watch_file:
            print(f"\nMonitoring: {self.watch_file}")

    def run_cycle(self):
        """Run one complete build→verify→analyze cycle"""
        self.print_header(f"CLAUDE CODE ITERATION CYCLE - {self.variant.upper()}")

        # Step 1: Build
        if not self.run_build():
            print(colored("\n✗ Build failed - fix compilation errors first", "red"))
            return False

        # Step 2: Verify
        success = self.run_verification()

        # Step 3: If mismatch, analyze
        if not success:
            self.analyze_differences()

        # Step 4: Summary
        self.print_summary(success)

        return success

    def watch_mode(self):
        """Watch a file and rebuild on changes"""
        if not self.watch_file:
            print(colored("No file specified for watch mode", "red"))
            return

        watch_path = Path(self.watch_file)
        if not watch_path.exists():
            print(colored(f"Watch file does not exist: {watch_path}", "red"))
            return

        print(colored(f"\n👁 Watching {watch_path} for changes...", "cyan"))
        print(colored("Press Ctrl+C to stop\n", "cyan"))

        last_mtime = watch_path.stat().st_mtime

        try:
            while True:
                time.sleep(1)
                current_mtime = watch_path.stat().st_mtime

                if current_mtime != last_mtime:
                    print(colored(f"\n🔄 Change detected in {watch_path.name}", "cyan"))
                    last_mtime = current_mtime
                    time.sleep(0.5)  # Debounce
                    self.run_cycle()
                    print(colored(f"\n👁 Watching {watch_path} for changes...", "cyan"))

        except KeyboardInterrupt:
            print(colored("\n\n👋 Watch mode stopped", "cyan"))


def main():
    parser = argparse.ArgumentParser(
        description="Claude Code Iteration Helper - Automates build→verify→analyze cycle"
    )

    parser.add_argument(
        "--variant",
        choices=["main_exe", "vr_exe", "dev_exe"],
        default="main_exe",
        help="Build variant to compile (default: main_exe)"
    )

    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="Show verbose output including build logs"
    )

    parser.add_argument(
        "-w", "--watch",
        metavar="FILE",
        help="Watch a source file and rebuild on changes (e.g., ../source/libgv/file.c)"
    )

    parser.add_argument(
        "-f", "--function",
        metavar="NAME",
        help="Focus on a specific function (for future use)"
    )

    args = parser.parse_args()

    verifier = BuildVerifier(
        variant=args.variant,
        verbose=args.verbose,
        watch_file=args.watch
    )

    if args.watch:
        # Watch mode - continuous
        verifier.watch_mode()
    else:
        # Single run
        success = verifier.run_cycle()
        sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
