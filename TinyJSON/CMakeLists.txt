add_library(TinyJSON STATIC
        Exception.h
        Reader.cc Reader.h
        Writer.h
        Value.h
        Document.h
        noncopyable.h
        PrettyWriter.h ReadStream.h WriteStream.h)
install(TARGETS TinyJSON DESTINATION lib)

set(HEADERS
        Document.h
        Exception.h
        noncopyable.h
        PrettyWriter.h
        Reader.h
        Value.h
        Writer.h
        )
install(FILES ${HEADERS} DESTINATION include)