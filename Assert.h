#ifndef NDEBUG
#   define ASSERT(condition, message) \
    if (! (condition)) { \
        std::cerr << "Failed ASSERT." << '\n' << message << '\n'; \
        std::terminate(); \
    }
#else
#   define ASSERT(condition, message) {}
#endif