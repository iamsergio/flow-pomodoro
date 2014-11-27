// Dummy version.h, just for tests

#ifndef FLOW_VERSION_H_TESTS
#define FLOW_VERSION_H_TESTS

namespace Version {
    const char *const last_commit_date = "";
    const char *const last_commit_sha1 = "";
    const char *const current_branch   = "master";
    const bool isTag                   = false; // If HEAD is a tag
    const bool hasGit                  = false; // If .git is present
}

#endif
