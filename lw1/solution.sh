#!/usr/bin/env bash
# Если любая из следующих команд завершится неудачей, скрипт прекратит свою работу
set -e

# Создаем каталог out, если он не существует
mkdir -p out

# Проверяем наличие каталога out и удаляем его содержимое, если он существует
if [ -d out ]; then
    rm -rf out/*
fi

# Переходим в каталог out
cd out

# Создаем файл me.txt с именем текущего пользователя
whoami > me.txt

# Копируем содержимое файла me.txt в metoo.txt
cp me.txt metoo.txt

# Создаем файл wchelp.txt с документацией о команде wc
man wc > wchelp.txt

# Выводим содержимое файла wchelp.txt в консоль
cat wchelp.txt

wc -l wchelp.txt
# Сохраняем количество строк в файле wchelp.txt в файл wchelp-lines.txt, -l говорит, что это количество строк, -d - разделитель -- пробел
wc -l wchelp.txt | cut -d' ' -f1 > wchelp-lines.txt

# Создаем файл wchelp-reversed.txt с содержимым wchelp.txt в обратном порядке
tac wchelp.txt > wchelp-reversed.txt

# Создаем файл all.txt, объединяющий содержимое всех файлов
cat wchelp.txt wchelp-reversed.txt me.txt metoo.txt wchelp-lines.txt > all.txt

# Создаем архив result.tar с файлами .txt -cf создать файл
# shellcheck disable=SC2035
tar -cf result.tar *.txt

# Сжимаем архив result.tar в result.tar.gz
gzip result.tar

# Выходим из каталога out
cd ..

# Проверяем наличие файла result.tar.gz в текущем каталоге
if test -f result.tar.gz; then
    rm result.tar.gz
fi

# Перемещаем новый файл result.tar.gz в текущий каталог
mv out/result.tar.gz .