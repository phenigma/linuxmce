// Do NOT edit this file, it is automatically generated prior to build.
#ifndef VERSION
#define VERSION "<=version=>"
#endif

#ifndef LMCE_version_h
#define LMCE_version_h
// In source files stored in archives and packages, these 2 lines will have the release version (build)
// and the svn revision as a global variable that can be inspected within a core dump
#ifdef EXTERN_REVISION_DATE
extern const char *g_szCompile_Date;
extern const char *g_szSourceRevision;
#else
const char *g_szCompile_Date="<=compile_date=>";
const char *g_szSourceRevision="<=source_revision=>";
#endif
#endif
