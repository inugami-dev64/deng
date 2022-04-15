# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: FetchDependencies.py - Python script to fetch all required dependencies for build
# author: Karl-Mihkel Ott

import os
import sys
import subprocess
from pathlib import Path

# Version definitions
TARGET_DIR = 'deps'
IMGUI_COMMIT_HASH = '28eabcb0994be436d5ab3bf62761f35a2a4eea1a'
NEKOWIN_COMMIT_HASH = '62e4efcca290e14efb8b3d0c9ef2714e69776877'
LIBDAS_COMMIT_HASH = '603e182cd6a45882b44bd7ea356a6371651b9151'
SHADERC_COMMIT_HASH = '96b1dd72a827304817470274a470c4d3b2293451'

# Dependency URLs
IMGUI_GIT_URL = 'https://github.com/ocornut/imgui'
NEKOWIN_GIT_URL = 'https://git.sadblog.xyz/nekowin'
LIBDAS_GIT_URL = 'https://git.sadblog.xyz/libdas'
SHADERC_GIT_URL = 'https://github.com/google/shaderc'


# Check if target directory is present and if it isn't create new directory
def MakeTargetDir():
    if(not os.path.isdir(TARGET_DIR)):
        os.mkdir(TARGET_DIR)


# Find git executable to use
def FindGitExecutable():
    search_list = ['git', 'git.exe', 'git.bat']

    devnull = open(os.devnull, 'w')
    for git in search_list:
        try:
            subprocess.call([git, '--version'], stdout=devnull)
        except (OSError,):
            continue
        return git

    raise Exception("Could not find git executable")


GIT_EXECUTABLE = FindGitExecutable()
PYTHON_EXECUTABLE = Path(sys.executable).resolve()


def CloneByCommitId(repo_url, commit_id):
    repo_name = repo_url[repo_url.rfind('/') + 1:]
    devnull = open(os.devnull, 'w')
    print(f"{repo_name} @ {commit_id}")

    subprocess.call([GIT_EXECUTABLE, 'clone', '--quiet', repo_url, f"{TARGET_DIR}/{repo_name}"], stderr=devnull)
    
    if subprocess.call([GIT_EXECUTABLE, 'checkout', '--quiet', commit_id], cwd=f"{TARGET_DIR}/{repo_name}", stderr=devnull) != 0:
        raise Exception(f"Invalid commit id for repo '{repo_name}'")

    devnull.close()


MakeTargetDir()
CloneByCommitId(IMGUI_GIT_URL, IMGUI_COMMIT_HASH)
CloneByCommitId(NEKOWIN_GIT_URL, NEKOWIN_COMMIT_HASH)
CloneByCommitId(LIBDAS_GIT_URL, LIBDAS_COMMIT_HASH)
CloneByCommitId(SHADERC_GIT_URL, SHADERC_COMMIT_HASH)
subprocess.call([PYTHON_EXECUTABLE, 'utils/git-sync-deps'], cwd=f"{TARGET_DIR}/shaderc")