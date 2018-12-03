package main

import (
	"fmt"
	"log"
	"strconv"
	"database/sql"
	_ "github.com/go-sql-driver/mysql"
)

const (
	DB_HOST = "tcp(127.0.0.1:3306)"
	DB_NAME = "mytest"
	DB_USER = "custom"
	DB_PASS = "123"
)

type User struct {
	Id	int
	CP	string
	NN	string
}

func Open() *sql.DB {
	db, err := sql.Open("mysql", fmt.Sprintf("%s:%s@%s/%s", DB_USER, DB_PASS, DB_HOST, DB_NAME))
	if err != nil {
		panic(err.Error())
		log.Fatal(err)
	}
	return db
}

func UserById(id int) *User {
	db := Open()
	defer db.Close()
	row := db.QueryRow("select id,cellphone,nickname from user where id=?", id)
	user := new(User)
	row.Scan(&user.Id, &user.CP, &user.NN)
	return user
}

func main() {
	user := UserById(1)
	fmt.Println("id	:" + strconv.Itoa(user.Id) + "\nnickname	:" + user.NN + "\ncellphone :" + user.CP)
}

