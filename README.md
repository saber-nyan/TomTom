# ТомТом

В директории `backend/` находится серверная часть приложения на Си. <br/>
В директории `android-app/` находится клиентская часть приложения на Kotlin.

## Стэк технологий
* C
* [flatbuffers](https://google.github.io/flatbuffers/index.html)
* [PostgreSQL](https://www.postgresql.org/docs/11/libpq.html)
* [libwebsockets](https://libwebsockets.org/git/libwebsockets/tree/?h=v3.2-stable)

***

* Kotlin/Andoid
* [OkHttp](https://square.github.io/okhttp/)
* Proguard

## Уязвимости
* `INSERT` SQLi в request_contract
* Blind `SELECT` SQLi в login
* IDOR в edit_me: можно поставить себе флаг `admin` в true и видеть контракты всех пользователей при вызове list_contracts
* IDOR в check_contract: при запросе можно задать любой ID, принадлежность указанного ID к текущему пользователю не проверяется