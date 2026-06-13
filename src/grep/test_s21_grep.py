import shutil
import subprocess
from pathlib import Path

import pytest

DIR = Path(__file__).resolve().parent
BIN = DIR / "s21_grep"
REF = ["busybox", "grep"] if shutil.which("busybox") else None
RUN = {"capture_output": True, "text": True, "cwd": DIR}


def run(args, *, check_rc=True):
    s21 = subprocess.run([str(BIN), *args], **RUN)
    ref = subprocess.run([*REF, *args], **RUN)
    assert s21.stdout == ref.stdout
    assert s21.stderr == ref.stderr
    if check_rc:
        assert s21.returncode == ref.returncode


@pytest.fixture(scope="session", autouse=True)
def require_binary():
    if not BIN.is_file():
        pytest.skip(f"build binary file first")
    if REF is None:
        pytest.skip("busybox is required")


@pytest.mark.parametrize(
    "args",
    [
        ["for", "test_files/test_1_grep.txt"],
        ["-n", "for", "test_files/test_1_grep.txt"],
        ["-c", "for", "test_files/test_1_grep.txt"],
        ["-i", "int", "test_files/test_5_grep.txt"],
        ["-v", "for", "test_files/test_1_grep.txt"],
        ["-l", "for", "test_files/test_1_grep.txt", "test_files/test_2_grep.txt"],
        ["-e", "^int", "test_files/test_1_grep.txt"],
        ["-f", "test_files/test_ptrn_grep.txt", "s21_grep.c"],
        ["-h", "-n", "for", "test_files/test_1_grep.txt", "test_files/test_2_grep.txt"],
    ],
)
def test_matches_busybox_grep(args):
    run(args)


def test_stdin():
    text = "alpha\nbeta\nalpha again\n"
    s21 = subprocess.run([str(BIN), "alpha"], input=text, **RUN)
    ref = subprocess.run([*REF, "alpha"], input=text, **RUN)
    assert s21.stdout == ref.stdout
    assert s21.returncode == ref.returncode


def test_no_match_exit_code():
    run(["^zzz_not_found$", "test_files/test_0_grep.txt"])


def test_invalid_regex():
    s21 = subprocess.run([str(BIN), "[", "test_files/test_0_grep.txt"], **RUN)
    ref = subprocess.run([*REF, "[", "test_files/test_0_grep.txt"], **RUN)
    assert s21.returncode == ref.returncode == 2


def test_missing_file():
    run(["pattern", "no_such_file.txt"])
