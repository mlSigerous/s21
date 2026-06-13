import shutil
import subprocess
from pathlib import Path

import pytest

DIR = Path(__file__).resolve().parent
BIN = DIR / "s21_cat"
REF = shutil.which("cat")
RUN = {"capture_output": True, "text": True, "cwd": DIR}


def run(args, *, check_rc=True):
    s21 = subprocess.run([str(BIN), *args], **RUN)
    ref = subprocess.run([REF, *args], **RUN)
    assert s21.stdout == ref.stdout
    if check_rc:
        assert s21.returncode == ref.returncode


@pytest.fixture(scope="session", autouse=True)
def require_binary():
    if not BIN.is_file():
        pytest.skip(f"build binary file first")
    if REF is None:
        pytest.skip("system cat not found")


@pytest.mark.parametrize(
    "args",
    [
        ["test_files/test_1_cat.txt"],
        ["-n", "test_files/test_2_cat.txt"],
        ["-b", "test_files/test_4_cat.txt"],
        ["-s", "test_files/test_1_cat.txt"],
        ["-t", "test_files/test_3_cat.txt"],
        ["-n", "test_files/test_1_cat.txt", "test_files/test_2_cat.txt"],
    ],
)
def test_matches_system_cat(args):
    run(args)


def test_stdin():
    text = "hello\nworld\n"
    s21 = subprocess.run([str(BIN)], input=text, **RUN)
    ref = subprocess.run([REF], input=text, **RUN)
    assert s21.stdout == ref.stdout
    assert s21.returncode == ref.returncode


def test_missing_file():
    s21 = subprocess.run([str(BIN), "no_such_file.txt"], **RUN)
    ref = subprocess.run([REF, "no_such_file.txt"], **RUN)
    assert s21.returncode == ref.returncode == 1


def test_invalid_flag():
    s21 = subprocess.run([str(BIN), "-Z", "test_files/test_1_cat.txt"], **RUN)
    ref = subprocess.run([REF, "-Z", "test_files/test_1_cat.txt"], **RUN)
    assert s21.returncode == ref.returncode == 1


def test_double_dash_file():
    s21 = subprocess.run([str(BIN), "--", "test_files/test_5_cat.txt"], **RUN)
    ref = subprocess.run([REF, "--", "test_files/test_5_cat.txt"], **RUN)
    assert s21.stdout == ref.stdout
