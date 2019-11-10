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
* `INSERT` SQLi в [request_contract](https://gitlab.com/mctf/final-2019/tomtom/blob/master/backend/src/api/request_contract.c#L86)
* Blind `SELECT` SQLi в [login](https://gitlab.com/mctf/final-2019/tomtom/blob/master/backend/src/api/login.c#L23)
* IDOR в [edit_me](https://gitlab.com/mctf/final-2019/tomtom/blob/master/backend/src/api/edit_me.c#L9): можно поставить себе флаг `admin` в true и видеть контракты всех пользователей при вызове list_contracts
* IDOR в [check_contract](https://gitlab.com/mctf/final-2019/tomtom/blob/master/backend/src/api/check_contract.c#L7): при запросе можно задать любой ID, принадлежность указанного ID к текущему пользователю не проверяется