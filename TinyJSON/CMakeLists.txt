add_library(TinyJSON STATIC
        Exception.h
        Reader.h
        Writer.h
        Value.h
        Document.h
        noncopyable.h
        ReadStream.h WriteStream.h)
install(TARGETS TinyJSON DESTINATION lib)

set(HEADERS
        Document.h
        Exception.h
        noncopyable.h
        Reader.h
        Value.h
        Writer.h
        )
install(FILES ${HEADERS} DESTINATION include)