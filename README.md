# VideoAssessment
### Утилита предоставляется на условиях "как есть". Вы используете утилиту на свой страх и риск. Автор не несёт никакую ответственность за возможные издержки в следствие неправильной работы утилиты.

## Основные сведения
* Целевая ОС: Linux
* Язык программирования: C++ (QT Library)
* Среда разработки: QT Creator
* Требуемое ПО: bash, QT, ffmpeg

## Поддерживаемые методы объективной оценки потерь качества при компрессии цифрового видео
* Математическое ожидание разности
* Метрика ***sup(x - y)***
* PSNR
* SSIM

## Поддерживаемые форматы входных данных
* AVI
* MP4
* MKV
* FLV

## Особенности работы
* Шаг оценки - 1 секунда
* В результате работы генерируется CSV-файл, содержащий колонки "Время" и "Значение". Файл можно открыть, например, с помощью MS Office Excel
* НЕ РЕКОМЕНДУЕТСЯ вводить заведомо неверные значения
