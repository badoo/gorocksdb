package gorocksdb

// #include <stdlib.h>
// #include <string.h>
// #include "rocksdb/c.h"
// #include "gorocksdb_iter.h"
import "C"
import (
	"unsafe"
)

type Values struct {
	c *C.gorocksdb_values_t
}

func NewValues(c unsafe.Pointer) *Values {
	return &Values{(*C.gorocksdb_values_t)(c)}
}

func (v *Values) Get(num int) ([]byte, int) {
	var cLen C.size_t
	cValue := C.gorocksdb_values_get(v.c, C.size_t(num), &cLen)
	if cValue == nil {
		return nil, 0
	}

	return charToByte(cValue, cLen), int(cLen)
}

func (v *Values) Len() int {
	cLen := C.gorocksdb_values_length(v.c)
	return int(cLen)
}

func (v *Values) Free() {
	C.gorocksdb_values_free(v.c)
	v.c = nil
}
