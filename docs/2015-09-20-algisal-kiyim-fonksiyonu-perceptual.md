---
layout: post
title: Algısal Kıyım Fonksiyonu (Perceptual Hashing)
date: '2015-09-20T20:06:00.000+03:00'
author: Bahri ABACI
categories:
- Veri Analizi
- Görüntü İşleme Uygulamaları
modified_time: '2015-09-20T20:07:44.978+03:00'
thumbnail: /assets/post_resources/perceptual_hashing/thumbnail.png
---

Hashing yada bir diğer deyişle kıyım fonksiyonu büyük bir veri kümesini,
verinin dağılımını kullanarak çok daha küçük veri boyutlarında ifade
etmek için kullanılan bir yöntemdir. Kıyım fonksiyonunda amaç veriyi
sıkıştırmak değil, verinin iletim veya saklanma sırasında bozulup
bozulmadığını anlayabilmektir. Öyleki verinin bir bitinde bile bir hata
oluşsa, çok çok yüksek olasılıkla kıyım fonksiyonunun çıktıları farklı
olacağından, veride meydana gelen değişiklik kolaylıkla
anlaşılabilecektir. Ancak bu durumun tersi her zaman geçerli değildir.
Yani kıyım fonksiyonu yanıtları aynı olan iki veri özdeştir diyemeyiz.
Bunu kolay bir örnekle anlamaya çalışalım. Farz edelim ki  "merhaba
dünya"  metninin iletmek istiyoruz.  

<!--more-->
  
Bu örnek için kıyım fonksiyonunu, **F(m) = sesli harflerin sayısı x sessiz harflerin sayısı + metin uzunluğu** olarak belirleyelim. Bu durumda kıyım değerimiz F = 5x7 + 13 = 48 olacaktır. Burada dikkat edilirse bir harfin eksik iletilmesi, sesli bir harfin sessiz, sessiz bir harfin sesli bir harf ile değişmesi durumunda kıyım değeri değişecektir ve orjinal metnin
bozulduğunu anlamamızı sağlayacaktır. Ancak bozulma sesli harflerin
sesli, sessiz harflerin sessiz harflerle değişmesi şeklinde olursa,
kıyım fonksiyonumuz meydana gelen bozulmayı anlayamayacaktır. 

Günlük hayatımızda çok fark etmesek de pek çok kritik programın
dağıtımında (işletim sistemi, bios update, vs.) daha üstün kıyım
fonksiyonları MD5, SHA-1, CheckSum çalışarak üretilmiş kodlar da
verilmekte ve programları kurmadan önce kıyım değerlerinin doğruluğunun
kontrol edilmesi önerilmektedir. Böylece indirme sırasında bir hata
oluşmuşsa, bu hata tespit edilecek ve kurulumun ortasında meydana
gelebilecek olası geri dönülemez bir hata önlenecektir.

Sürekli görüntü işleme üzerine yazıların olduğu bir ortamda konunun
görüntü işleme ile ilgisini hemen anlayamamış olabilirsiniz. Ancak kıyım
fonksiyonunun özel bir türü olan **Algısal Kıyım** (Perceptual Hashing)
görüntü işleme ve analizinde oldukça önemli bir yer tutmaktadır. Algısal
kıyımda da amaç büyük bir verinin (imge veya video) kıyım değerini
bularak ve veriyi bu kıyım değeri ile ifade etmektir. Klasik kıyım
yöntemlerinden farklı olarak algısal kıyımda, veride meydana gelen
değişikliklerin, verinin algısal bütünlüğü korunduğu sürece değişmemesi
istenmektedir. Algısal bütünlüğü anlamak için aşağıdaki görsel faydalı
olacaktır.

![Algısal kıyım Fonksiyonu][perceptual_hashing]

Resimde görüldüğü gibi imge üzerinde meydana gelen boyut, ölçek, renk
değişimleri insan algısında çok büyük farklılıklar yaratmamakta ve tüm
imgelerin aynı imgeden türetilmiş olduğunu anlayabilmemizi
 sağlamaktadır.  
  
Algısal kıyımlamada da hedef yukarıda verilen tüm imgeler için aynı
çıktıyı üreten bir kıyım fonksiyon bulmaktır. Böylece telif hakları
bulunan bir imgenin herhangi bir yerde kullanılıp kullanılmadığı, veya
yayınlanması yasak olan imge/video içeriklerinin herhangi bir ortamda
yayınlanıp yayınlanmadığı gibi emek gerektircek işler otomatik olarak
yapılabilinecektir. Ayrıca fotoğraf yükleme/saklama siteleri de böyle
bir yöntemi koşturarak, yüklenen içeriğin hali hazırda sistemlerinde
kayıtlı olup olmadığını analiz ederek, aynı fotoğrafın defalarca
saklanmasını engellemektedir.  
  
Algısal kıyım için çok farklı yöntemler bulunmaktadır. Bu yöntemlerden
en çok bilinen ve muhtemelen en kolay olan yöntem A-hash, ortalama kıyım
fonksiyonudur. Bu fonksiyon parametre olarak aldığı imgeden 64 bit
uzunluklu bir (long long türünde) tam sayı döndürmektedir. Algoritmanın
basamaklarını adım adım ineceleyelim.  
  

    1.  Resmi 8x8 boyutuna ölçekle ve gri tonla
    2.  64 imge gözeneğinin ortalamasını bul
    3.  Ortalamadan büyük gözeneklere 1, küçüklere 0 yaz
    4.  8x8 ikili veriyi 64 bitle kodla

  
İlk adımda imge boyu ne olursa olsun, oldukça küçük bir boyuta 8x8
indirgeniyor. Bu adım kopya imgeye yapılan ölçekleme girişimlerinin
etkisini kaldırmak için kullanılyor. Ardından imge gri tonlanarak renk
etkileri azaltılyor.  
  
İkinci adımda ise gözeneklerin ortalaması bulunuyor. Bu ortalama üçüncü
adımda eşikleme için kullanıldığından, parlaklık azaltma veya artırma
gibi girişimlerin etkisi de bu adımda yok ediliyor.  
  
Son adımda ise üçüncü adımda elde edilen ikili imge eşsiz bir sayıya
dönüştürülüyor ve imgeden bir tam sayı elde ediliyor. A-kıyım yöntemi
için yazılan fonksiyon aşağıda verilmiştir.

```c
long long Ahash(BMP I) 
{
    int M = 8;
    int N = 8;
    double *IMxN = (double*) malloc(M*N*sizeof(double));

    fIMxN(I, IMxN, M, N);

    long long ahash = 0;
    int j;
    double meanI = 0;

    for(j=0; j < M*N ;j++) { meanI += IMxN[j]; }
    meanI = meanI/(M*N);

    // Bu asamada M=8, N=8 olmalı
    for(j=0; j < M*N ;j++) 
    {
        ahash = ahash << 1 | IMxN[j] > meanI;
    }

    free(IMxN);
    return ahash;
}
```

Kod içerisindeki `fIMxN` fonksiyonu parametre olarak aldığı `I` imgesini gri seviye kodlayıp ve MxN boyutlarına kodlama işlemi için yazılmıştır. Bu işlem sonucu oluşan 8x8 imge ise `IMxN` vektörü içerisinde döndürülmüştür.  
  
Kodlama kısmında karmaşık görünün ikinci `for` döngüsü algoritmanın üç ve dördüncü adımlarını gerçekleştirmektedir.  `IMxN` vektöründe saklanan veriler ortalama ile karşılaştırılarak `ahash` tam sayısının j. bitine yazılmaktadır.  
  
Bu fonksiyonun ürettiği ahash tam sayısının sayı değeri olarak bir anlamı yoktur. Görüntü ile ilgili bilgiler sayının bitlerinde saklandığından, benzer imgelerden üretilmiş iki sayının karşılaştırılması da bit bit yapılmalıdır. Programlamada iki sayının bit bit farkının bulunması işlemine Hamming uzaklığı adı verilir. Bu uzaklığı bulmak için aşağıdaki gibi bir fonksiyon yazmak gerekmektedir.

```c
char HammingDist(long long x, long long y)
{
    char dist = 0;
    long long val = x^y;// xor yap ve farki bul
    int i;
    long long t = 1;

    for (i=0; i < 64; i++) {
        dist += (char) ( (val >> i) & t );
    }
    return dist;
}
```

Kod basitçe x ve y değişkenlerinin xor değerini almakta (ikili çıkarma
işlemi) ve iki bit dizisi arasındaki uyuşmayan bitleri bulmaktadır.
Ardından 64 bit gezilerek uyuşmayan bitlerin sayısı uzaklık olarak
döndürülmektedir (64 bit içerisinde en büyük uzaklık 64 olacağından
uzaklık tipi char olarak belirlenmiştir.).  
  
Artık test kısmına geçebiliriz. Test için seçilen dört imge aşağıda
verilmiştir. Amacımız ilk sıradaki imge ile üretilen kıyım değerinin
diğer üç imge ile üretilen kıyım değerlerine olan benzerliğini bulmak.  
  
![Algısal Kıyım İle Resim Benzerliğinin Ölçülmesi][sample]
  
Yapılan bozulmalarıda sıralayacak olursak ikinci imge, ilk imgenin
gri-sepya kodlanması ve ölçeklenmesi, üçüncü imge mozaik etkisi ve ölçek
değişimi ile üretilmiştir. Son imge ise rastgele seçilen bir imgenin
üreteceği sonuçları göstermek için seçilmiştir. Benzerlik için bulunan
sonuçlar 64 bit karşılaştırmasında tutarlı olan bitlerin yüzde olasılığı
olarak hesaplanmıştır. Bu durumda elde edilen değerler imge1-imge2
arasında %93.75, imge1-imge3 arasında %95.31 ve imge1-imge4 arasında ise
%73.44 olarak hesaplanmıştır. 64 bit kodlama ile elde edilen benzerlik
sonuçları oldukça yeterli görünse de imge1-imge4 benzerliğinin %75
seviyesinde olması kıyım algoritmasının zayıflığının bir
göstergesidir.  
  
Yazımızın başında da belirttiğimiz gibi A-kıyım yöntemi oldukça basit
bir yöntemdir. Algoritmanın bu zayıflığı D-kıyım yöntemi ile kısmen
giderilebilmektedir. D-kıyım yönteminde gözeklerin ortalamadan büyük
veya küçük olduğu değil, her bir gözeğin bir yanındaki gözekten küçük
veya büyük olduğu bilgisi saklanmaktadır. D-kıyım yöntemi için yazılan
kod aşağıda verilmiştir.

```c
long long Dhash(BMP I) 
{
    int M = 8;
    int N = 9;
    double *IMxN = (double*) malloc(M*N*sizeof(double));

    fIMxN(I, IMxN, M, N);

    long long dhash = 0;

    int i, j;
    double meanI = 0;

    for(i=0; i < M ;i++) 
    {
        // Bu asamada M=8, N=8 olmalı
        for(j=1; j < N ;j++) 
        {
            dhash = (dhash << 1) | (IMxN[i*N+j] < IMxN[i*N+j-1]);
        }
    }

    free(IMxN);
    return dhash;
}
```

Koddan da görüldüğü üzere algoritmanın A-kıyım yönteminden tek
farkı `IMxN\[j\]  < meanI` işelmi yerine `IMxN\[i\*N+j\] < IMxN\[i\*N+j-1\]` işleminin yapılmasıdır. Ayrıca bu işlemin hatasız bir şekilde yapılabilmesi için imge 8x8 yerine 8x9 boyutlarına küçültülmüştür.  
  
Aynı resimler için D-kıyım yönteminin incelersek, imge1-imge2 arasında
%82.81, imge1-imge3 arasında %87.50 ve imge1-imge4 arasında %57.81
benzerlik bulunduğunu görürüz. Algoritmayı A-kıyım yöntemi ile
karşılaştırdığımızda D-kıyım yönteminin yanlış eşleşmeyi çok daha iyi
ayırt ettiğini görebiliriz. Ancak D-kıyım yönteminin de "en iyi yöntem"
olmadığı imge1-imge4 arasındaki %57 benzerlikten görülebilir.  
  
  
Güncel çalışmalarda bu yöntemlerin çok daha karmaşık türeveri olan
P-kıyım, Radish ve Wavelet tabanlı kıyım gibi yöntemler
kullanılmaktadır. Bu yazıda verilmeyen diğer alternatifleri test etmek
için [bu bağlantıyı](http://www.phash.org/demo/) kullanabilirsiniz.  
  
Ayrıca yazıyı hazırlama aşamasında sıklıkla yararlandığım [bu bağlantıdan](http://www.hackerfactor.com/blog/?/archives 432-Looks-Like-It.html) da yazıyı farklı bir anlatım tarzıyla okuyabilirsiniz.  
  
**Referanslar**
* [Hackerfactor blog yazısı](http://www.hackerfactor.com/blog/?/archives/432-Looks-Like-It.html)

[RESOURCES]: # (List of the resources used by the blog post)
[perceptual_hashing]: /assets/post_resources/perceptual_hashing/algisal_kiyim.png
[sample]: /assets/post_resources/perceptual_hashing/kiyim_test.png