package main

// #include <unistd.h>
// #include "loadsym.h"
// #cgo LDFLAGS: -ldl -lpthread -lz
// #cgo CFLAGS: -D_GNU_SOURCE
import (
	"C"
)
import (
	"fmt"
	"unsafe"
)

func main() {
	fmt.Printf("hello")
}
func Println(str string) {
	C.debug(C.CString(str))
}

//export go_write
func go_write(fd C.int, buf unsafe.Pointer, count C.size_t) C.ssize_t {
	return C.true_write(fd, buf, count)
}
