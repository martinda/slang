# Build using docker

```
docker build --tag slang .
rm -rf build
docker run -v $(pwd):/slang -ti slang /bin/bash
```

Once inside the container:
```
scripts/build_clang.sh
```

It will fail with:
```
/slang/source/../include/slang/text/SourceManager.h:10:10: error: 'filesystem' file not found [clang-diagnostic-error]
#include <filesystem>
         ^
```

Not sure what the problem is.
