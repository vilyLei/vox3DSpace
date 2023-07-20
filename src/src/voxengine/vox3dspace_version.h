#ifndef VOX3DSPACE_ENGINE_VERSION_H_
#define VOX3DSPACE_ENGINE_VERSION_H_

namespace voxengine {

// Vox3DSpace version is comprised of <major>.<minor>.<revision>.
static const char kVox3DSpaceVersion[] = "1.0.1";

const char *Version() { return kVox3DSpaceVersion; }

}  // namespace voxengine

#endif  // VOX3DSPACE_ENGINE_VERSION_H_
