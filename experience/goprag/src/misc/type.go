package main

import (
	"fmt"
)

type Stringer interface {
	String() string
}

func ft(v interface{}) string {
	if str, ok := v.(string); ok {
		return str
	} else if str, ok := v.(Stringer); ok {
		return str.String()
	} else {
		fmt.Println("value is not string or Stringer")
		return ""
	}
}

func main() {
	var str string = "abcd"
	fmt.Println(ft(str))
	iv := 33
	fmt.Println(ft(iv))
}
