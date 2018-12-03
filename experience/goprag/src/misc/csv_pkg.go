package main

import (
"encoding/csv"
"log"
"os"
"fmt"
"stringutil"
)

func check(e error){
if(e != nil){
panic(e)
}
}

func main(){
records := [][]string{
{"aa", "bb", "cc"},
{"dd", "ee", "ff"},
{"gg", "hh", "jj"},
}

/*f, err := os.Create("./tmp.csv")
check(err)
defer f.Close()
*/

rd := []string{"aabb", "ccdd"}
w := csv.NewWriter(os.Stdout)
//w2 := csv.NewWriter(f)
w.WriteAll(records)
//w2.WriteAll(records)
w.Write(rd)
w.Flush()
//w2.Write(rd)
//w2.Flush()

if err := w.Error(); err != nil {
log.Fatalln("error writting csv:", err)
}
/*if err := w2.Error(); err != nil {
log.Fatalln("error writting csv to f:", err)
}*/
fmt.Println(stringutil.Reverse("0123456789"))
}
