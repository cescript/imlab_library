---
layout: post
title: IMLAB Görüntü İşleme Kütüphanesi
date: '2018-12-03T19:41:00.000+03:00'
author: Bahri ABACI
categories:
- Görüntü İşleme
modified_time: '2019-07-14T23:08:49.580+03:00'
thumbnail: /assets/post_resources/imlab_library/thumbnail.png
---

Lisans üçüncü sınıfta dönem projesi geliştirmesi için başladığım C ile görüntü işleme çalışmalarım yıllar içerisinde mini bir görüntü işleme kütüphanesine dönüştü. Bu blogda da bilinen adı ile CBMP bu kütüphanenin ilk ve amatör halini oluşturmakta idi. İlerleyen dönemlerde kişisel projeler ve çalışmalarımda da CBMP benzeri küçük ve işlevsel bir görüntü işleme kütüphanesine sık sık ihtiyaç duydum. CBMP kütüphanesinin veri tiplerinin az olması, renksiz görüntü tipinin olmaması ve orta ölçekli bir projede ihtiyaç duyulan bellek yönetimi ve tip esnekliğine sahip olmaması nedenlerinden ötürü CBMP kütüphanesini bir noktada terk ederek; organizasyon ve hiyeraşisi üzerine daha çok düşündüğüm, yazımı ve okunurluğu üzerine ciddi çalışmalar yaptığım ve basit veri tipleri ile fonksiyonları tek bir isim altında toplayabildiğim [IMLAB](https://github.com/cescript/imlab_library) kütüphanesini yazmaya karar verdim.

<!--more-->

Bu yazımda geliştirmesine uzun süredir devam ettiğim ve belirli bir olgunluğa eriştiğini düşündüğüm (hala hatalarla dolu olsa da) [IMLAB](https://github.com/cescript/imlab_library) kütüphanesinin kısa bir tanıtımını yapacağım. Blogda paylaşacağım yeni yazılarda da CBMP yazımından çıkıp [IMLAB](https://github.com/cescript/imlab_library) fonksiyonları ile devam edeceğim. Yazıya devam etmeden önce [IMLAB](https://github.com/cescript/imlab_library) adresinden kütüphanenin son sürümünü indirebilir ve test projeleri ile kütüphaneyi kullanmaya başlayabilirsiniz. Ayrıca [https://github.com/cescript](https://github.com/cescript) bağlantısından blogda daha önce paylaşımını yaptığım bazı örnek projelerin [IMLAB](https://github.com/cescript/imlab_library) ile gerçeklenmesine de bakabilirsiniz.

[IMLAB](https://github.com/cescript/imlab_library) 7 farklı bileşenden (görüntü işleme, lineer cebir, algoritma, makine öğrenmesi, bilgisayarlı görü, olasılık ve istatistik, dosya işlemleri) oluşan bir görüntü işleme kütüphanesidir. Bu 7 farklı kütüphane matris, vektör, dizi gibi temel tanımlamaların yapıldığı çekirdek kütüphaneyi kullanmaktadır.

IMLAB çekirdek kütüphanesi `matrix_t`, `vector_t`, `array_t` ve `string_t` olmak üzere 4 temel veri tipi tanımlamaktadır. Bu tiplerden `matrix_t` ve `vector_t` yapılarının detaylı açıklaması bu yazıda verilecektir. Diğer veri yapıları ve fonksiyonların açıklamaları da yeni yazılarla beraber yapılacaktır.


`matrix_t` iki boyutlu verilerin tutulması, saklanması ve okunması için oluşturulan veri tipidir. C programlama dilinin desteklediği tüm standart veri tipleri (char, short, int, ..) ile oluşturulabilir.

```c
// sadece veri tipini kullanarak 0x0 matris yarat
matrix_t *out = matrix_create(uint32_t);

// 1024x1024 buyuklugunde float tipinde bir matris yarat
matrix_t *out = matrix_create(float, 1024, 1024);

// 9 elemanli bir dizi yarat
double numbers[9] = {1,2,3, 4,5,6, 7,8,9};
// dizi elemanlarini kullanarak 3x3x1 boyutlarinda bir matris yarat
matrix_t *out = matrix_create(double, 3,3,1, numbers);

// daha once yaratilan bir matrisin kopyasini (veriyi kopyalama) olustur
matrix_t *out_copy = matrix_create(out, NULL);

// daha once yaratilan bir matrisin kopyasini (veriyi kopyala) olustur
matrix_t *out_clone = matrix_create(out, data(out));
```
Oluşturulan matris yapısı bir bellek üzerinde tip, satır, sütun, kanal ve veri bilgisini saklamak için gerekli bir bellek ayırmakta ve bu alanın başını işaret etmektedir. Matris yapısına veri eklemek ve çıkarmak istendiğinde aşağıda yer alan iki yöntem kullanılabilir.

```c
// veri işaretcisini kullanarak erisim
matrix_t *out = matrix_create(float, 1024, 1024);
// out matrisinin veri işaretcisini al
float *matrix_data = mdata(out, 0);

// out matrisinin verileri uzerinde calis
for(size_t i = 0; i < volume(out); i++) {
    float val = matrix_data[i];
}

// at yöntemi ile erişim
matrix_t *out = matrix_create(float, 1024, 1024);

// out matrisinin (r,c) hucresindeki veriyi al
for(size_t r = 0; i < rows(out); r++) {
    for(size_t c = 0; c < cols(out); c++) {
        float val = at(float, out, r,c);
    }
}
```

Kütüphanede imgelerin saklanması için özel bir veri yapısı oluşturulmamış, bunun yerine uint8_t veri tipi ile oluşturulan bir matrix_t yapısı tercih edilmiştir. Aşağıda matrix_t yapısının imge olarak kullanımına ait bir örnek verilmiştir.

```c
#include "imcore.h"

int main(int argc, unsigned char *argv[]) {

    int r,c;

    // 50x800 boyutunda bir imge yarat
    matrix_t *image = matrix_create(uint8_t, 50, 800, 3);

    // her piksel uzerinden gec ve renkleri ayarla
    for(r = 0; r < rows(image); r++)
    {
        for(c = 0; c < cols(image); c++)
        {
            // renk gecisini yarat
            struct color_t rgb = HSV(c * 256 / cols(image), 255, 255);
            // piksel degerini ayarla
            at(uint8_t, image, r, c, 0) = rgb.blue;
            at(uint8_t, image, r, c, 1) = rgb.green;
            at(uint8_t, image, r, c, 2) = rgb.red;
        }
    }

    imwrite(image, "hsv_gradient.bmp");

    // gereksiz bellegi temizle
    matrix_free(&image);

    return 0;
}
```
Verilen örnek kod; 50x800 büyüklüğünde renkli (3 kanallı) bir imge oluşturmakta ve imgenin sütunlarını HSV renk uzayında H değerini değiştirerek boyamaktadır. Verilen kod parçası çalıştırıldığında aşağıda verilen imge oluşmaktadır.

<div class="separator" style="clear: both; text-align: center;"><a href="https://3.bp.blogspot.com/-zirfPMGH4bc/XAVLsUp-E5I/AAAAAAAABtE/2pjikzih6KAgGTwG4xVH4sQmzAiTBhdRgCLcBGAs/s1600/hsv_gradient.png" imageanchor="1" style="margin-left: 1em; margin-right: 1em;"><img border="0" data-original-height="50" data-original-width="800" height="40" src="https://3.bp.blogspot.com/-zirfPMGH4bc/XAVLsUp-E5I/AAAAAAAABtE/2pjikzih6KAgGTwG4xVH4sQmzAiTBhdRgCLcBGAs/s640/hsv_gradient.png" width="640" /></a></div>


`vector_t` bir boyutlu ve uzunluğu bilinmeyen verilerin tutulması, saklanması ve okunması için oluşturulan veri tipidir. C programlama dilinin desteklediği tüm standart veri tipleri (`char`, `short`, `int`, ..) ve kodlayıcı tarafından yaratılan yapılar ile oluşturulabilir.

```c
// sadece veri tipini kullanarak 1 uzunluklu vektör yarat
vector_t *out = vector_create(uint32_t);

// 1024 uzunluklu float tipinde bir vektör yarat
vector_t *out = vector_create(float, 1024);

// 9 elemanli bir dizi yarat
double numbers[9] = {1,2,3, 4,5,6, 7,8,9};
// dizi elemanlarini kullanarak 9 uzunluklu bir vektör yarat
vector_t *out = vector_create(double, 9, numbers);
```

Matris yapısına benzer olarak, vektör yapısı da bir bellek üzerinde tip, uzunluk ve veri bilgisini saklamak için gerekli bir bellek ayırmakta ve bu alanın başını işaret etmektedir. Vektör yapısının elemanlarına matris yapısında kullanılan yöntemlere ek olarak `vector_push`/`vector_pop` yöntemleri ile de erişilebilir.

```c
// yeni bir vektor yarat
vector_t *out = vector_create(float);

float values[12] = {0.0 0.1, 0.3, 0.5, 0.7, 0.9, 0.8, 0.6, 0.5, 0.4, 0.2, 0.0};

// vektore yeni elemanlar ekle
for(size_t i = 0; i < 12; i++) {
    vector_push(out, &values[i]);
}
```
`vector_push` yöntemi, boyutu bilinmeyen vektör yapılarına yeni eleman eklemek için kullanılır. Yukarıda verilen örnekte `out` vektörü bir uzunluklu olarak yaratılmış ve for döngüsü içerisinde uzunluğu otomatik olarak artırılmıştır.

Vektör yapısı matris yapısına ek olarak kullanıcı tarafından oluşturulan yapılarla da oluşturulabilir. Aşağıda kullanıcı tarafından oluşturulan bir yapı ile vektör yapısının nasıl kullanılacağına dair bir örnek verilmiştir.

```c
// yeni bir veri yapisi olustur
struct person {
    uint32_t id;
    uint32_t birthday;
};

// yeni veri yapisini saklayacak bir vector yarat
vector_t *people = vector_create(struct person);

// vektore eklenecek bir eleman olustur
struct person var = {1, 1990};

// vektore ekle
vector_push(people, &var);
 ...
// vektoru temizle
vector_free(&people);
```

Yazıda paylaştığım veri yapılarına ek olarak görüntü işlemede sıklıkla ihtiyaç duyulan pek çok veri tipi olabildiğince sadeleştirilerek IMLAB kütüphanesine eklenmiştir. Ancak kütüphanede yer alan bu yapıların dokümantasyonunda eksikler bulunduğundan şu an için bu tiplerin kullanımı oldukça zordur. `matrix_t` ve `vector_t` yapıları için doxygen ile bir açıklama sayfası oluşturulmuş ve bu yapılar ile kullanılabilecek pek çok fonksiyon örnekler ile açıklanmıştır. Projenin dökümantasyonuna aşağıda verilen bağlantıdan ulaşabilirsiniz.

IMLAB Kütüphanesi: [https://github.com/cescript/imlab_library/blob/master/docs/refman.pdf](https://github.com/cescript/imlab_library/blob/master/docs/refman.pdf)

Proje ile ilgili görüş ve önerilerinizi [github](https://github.com/cescript/imlab_library/issues) üzerinden paylaşabilirsiniz.