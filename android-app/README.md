Для сборки необходимо в корневой директории проекта создать файл `keystore.properties` с содержимым:

```properties
# Пароль от хранилища
storePassword=meh
# Пароль от ключа в хранилище
keyPassword=heh
# Название ключа в хранилище
keyAlias=release
# Путь до хранилища ключей
storeFile=/home/seiba/secrets/android.p12
```

После этого открыть Android Studio или запустить сборку через
```bash
$ ./gradlew assembleRelease
```

***

*desu~*
