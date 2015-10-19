# facedetector
Simple facedetectors on OpenCV libs 249/300 for scan and show/write/test images in selected dir.
300_cuda version stable only CPU processing and unstable in GPU. Sometimes occur memory overflow and videodriver crash.
Builds 249/300 is stable and include all needs dll.
249 included only necessary dll, 300 - opencv_world.
300_cuda - version standalone not available, because it require opencv_cuda* libs and nvidia cuda dll - it more them 300Mb.
And it not noticeable increase speed of scan.
Project settings and another cascades available in sources folder.

Детектор лиц на библиотеках OpenCV 249 и 300. 
Версия 300_cuda стабильно работает только на CPU, при использовании графического процессора иногда случается переполнение памяти и падение видеодрайвера.
Билды 249/300 стабильны и содержат все необходимые для работы библиотеки.
Причем в 249 включены лишь необходимые библиотеки, а в 300м - opencv_world.
Это связано с тем, что у меня не компилируется либа под x86, делать ее только для 64 без CUDA нет смысла.
Библиотеки, скомпилированные с CUDA начинают тащить на собой NVIDIA, так что они непригодны для сборки x64.
CUDA-версии в сборках нет, так как она требует не только сотен мегабайт(!) библиотек OpenCV withCUDA,
но и dll-ок NVIDIA, делать ее standalone бессмысленно, также она нестабильна - падает на некоторых изображениях.
Никакого заметного прироста CUDA-версия не дает.
Настройки проектов и разные каскады содержатся в разделе sources. 

Желательно:
-собрать standalone билды x86 и x64 300х версий на раздельных либах
-сделать новый проект работы на GPU, используя  только opencv библиотеки. 