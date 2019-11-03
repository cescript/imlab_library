---
layout: post
title: İmge Renksizleştirme (Image Decolorization)
date: '2015-10-16T21:32:00.000+03:00'
author: Bahri ABACI
categories:
- Görüntü İşleme Uygulamaları
- Görüntü İşleme
modified_time: '2015-10-17T18:09:43.410+03:00'
thumbnail: /assets/post_resources/image_decolorization/thumbnail.png
---

Pek çok görüntü işleme uygulamasında (yüz, plaka, karakter tanıma gibi)
ve işleminde (kenar, anahtar nokta tespiti gibi) renk bilgisinden çok
doku bilgisine ihtiyaç duyulmaktadır.  Bu nedenle hem uygulamaların
hızlandırılması hem de işlem karmaşıklığının azaltılması için genellikle
renk bilgisi bir gri seviye dönüşüm ile kaldırılmaktadır. Dönüşüm
sonucunda üç farklı kanaldan (R,G,B) oluşan renkli sayısal bir imge, tek
bir kanala indirgenir ve renkler siyahın farklı tonları ile kodlanır.
Literatürde önerilen ve yaygın olarak kullanılan gri seviye dönüşümü,
Kırmızı, Yeşil ve Mavi kanallarının sırasıyla 0.299, 0.587 ve 0.114
ağırlıklarıyla çarpılıp, toplanmasıyla yapılmaktadır. Ancak böyle bir
dönüşüm doku bilgisinin tutulmasını garanti altına almamaktadır.

<!--more-->

Yukarıdaki imge durumu en iyi anlatan örneklerden biridir. İmgenin sol
yarısında görünen renkler, literatürde kullanılan ağırlıklı ortalama
yöntemi ile gri seviye kodlandığında, imgenin sağ yarısında da görüldüğü
üzere, aynı gri seviyeye sahip olmaktadırlar. Başka bir deyişle dönüşüm
sonucu balonun dokusu ve dolayısıyla her parçasının farklı renklerden
oluştuğu bilgisi kaybolmaktadır. 

Bu yazımızın başlığı olan "*imge renksizleştirme*" (image
decolorization) gri seviye dönüşüm sırasında doku bilgisinin de
korunmaya çalışıldığı içerik tabanlı bir görüntü işleme yöntemidir. Bu
konu başlığı altında çeşitli yöntemler bulunsa da bu yazımızda 2012
yılında Siggraph konferansında *Real-time Contrast Preserving
Decolorization* başlığı ile tanıtılan yöntemi bazı basitleştirmeler
yaparak ele alacağız. 

Bir imgedeki doku bilgisi imge gözekleri arasındaki fark ile tanımlanır.
İyi bir gri seviye dönüşüm işleminde gözekler arasındaki fark
olabildiğince korunmalıdır. Yani renkli imgede yer alan iki gözek
birbirinden farklı ise gri seviye dönüşüm sonucunda da bu iki gözek
birbirinden farklı, iki gözek birbirine benzer ise dönüşüm sonrası da
benzer olmalıdır. İfadeyi daha iyi anlatabilmek için aşağıda mavi ve
yeşil renklerden oluşan bir imge verilmiştir. Kırmızı ile işaretlenen
iki noktayı rastgele seçilen iki gözek olarak ele alırsak; renkli imgede
iki gözeğin birbirinden farklı olduğunu (birincisi mavi, ikincisi yeşil)
görürüz. Üçüncü sırada verilen imge ilk imgenin klasik ağırlıklı
ortalama yöntemi ile gri seviyeye kodlanmış halini göstermekte. Burada
birinci ve ikinci gözeklerin değerleri birbirlerine eşit olduğundan,
renkli resimde yer alan mavi-yeşil geçişi kaybedilmiştir. Son sırada
verilen imge ise *imge renksizleştirme* işleminin sonucunu
göstermektedir. Resimden de görüldüğü üzere dönüşüm sonrasında doku
bilgisi ve imge içeriği büyük ölçüde korunmuştur.

![Renk Körlüğü Test Patterni][image_decolorization]

İşlemi matematiksel olarak ifade etmek için rastgele seçilen iki noktayı
 $x$ ve $y$ olarak isimlendirelim. Gri seviye imge ($g$) tek kanaldan
oluştuğundan iki gözek arasındaki fark doğrudan $g_x-g_y$ işlemi ile
bulunacaktır. Renkli resimde de bu iki gözek arasındaki farka
$\delta_{x,y}$ diyelim. Amacımız imge içerisinden seçilebilecek tüm
$x,y$ çiftleri  için $g_x-g_y  =  \delta_{x,y}$ eşitliğini sağlayan
gri seviye dönüşümü bulmak. Bu eşitliği sağlamak oldukça güç olduğundan
amacımızı $g_x-g_y$ farkını olabildiğince  $ \delta_{x,y}$ farkına
yaklaştırmak olarak yenilersek, aradığımız gri seviye imge $g$; $$E(g) =
\sum_{(x,y)\in P} (g_x-g_y  -  \delta_{x,y})^2$$enerji
fonksiyonunu en aza indiren imgedir. Burada $P$ işlemin hızlandırılıması
için imge üzerinden rastgele seçilmiş  $x,y$ çiftleri havuzunu
göstermektedir.

Artık bilinmeyen değişkenleri ele alarak yöntemi kodlamaya başlayabiliriz. Denklemin ilk bilinmeyeni gri seviye imge $g$ dir. Basitlik olması açısından gri seviyeli imgenin ağırlıklı ortalama yöntemi ile oluştuğunu varsayabiliriz. Dolayısıyla $g$ imgesi matematiksel olarak $$g=w_rR+w_gG+w_bB$$şeklindedir. Burada klasik yöntemden farklı olarak kanal katsayıları sabit değildir ve enerji fonksiyonunu en azlayan ağırlık değerleri katsayı olarak seçilecektir.  
  
İkinci bilinmeyenimiz $\delta_{x,y}$, iki renk arasındaki uzaklığın ölçüsüdür. Yukarıdaki örnek imge üzerinden devam edersek, $\delta_{x,y}$ mavi rengin yeşile olan uzaklığını göstermektedir. Peki iki renk arasındaki uzaklığı nasıl ölçebiliriz?

  

Şüphesiz akla gelen ilk yöntem Öklid uzaklığını kullanmak olacaktır. Bu durumda uzaklık değerimiz; $$\delta_{x,y}=\sqrt{(R_x-R_y)^2 (G_x-G_y)^2+(B_x-B_y)^2}$$ifadesi ile bulunabilir. Aşağıda verilen tabloda renkler ilk sırada yer alan renge olan Öklid uzaklıklarına göre sıralanmıştır.

<table class="spectrum boxed">
<tbody>
<tr>     
<td style="background-color: #b1224c;">0.00</td>
<td style="background-color: #a56666;">16.71</td>
<td style="background-color: #8b228c;">16.85</td>
<td style="background-color: #64224c;">17.43</td>
<td style="background-color: #ff224c;">17.66</td>
<td style="background-color: #b10000;">18.85</td>
<td style="background-color: #b100e8;">36.15</td>
<td style="background-color: #d8afb4;">40.64</td>
<td style="background-color: #2cda30;">51.79</td>
<td style="background-color: #fcff00;">55.57</td>
</tr>
</tbody>
</table>

Yukarıdaki tablodan da görüldüğü üzere RGB uzayında Öklid uzaklığı
renklerin görsel olarak benzerliklerini hesaba katmamaktadır. Bu nedenle
iki rengin algısal benzerliklerini ölçmek için uygun değildir.
Uygulamalarda iki renk arası uzaklık ölçümü için genellikle farklı renk
uzayları (HSV, XYZ, Lab, vs.) kullanılmaktadır. Bunlardan en yaygın
kullanılanı Lab renk uzayıdır. Lab renk uzayı renkler arasındaki algısal
farklılıklar modellenerek oluşturulmuş bir renk uzayıdır. HSV dönüşümüne
benzer olarak Lab dönüşümü de doğrusal olmayan bir dönüşümdür. Dönüşüm
için yazılan fonksiyon aşağıda verilmiştir.

```c
void rgb2Lab(RGBA C1, float &L, float &a, float &b)
{
    float R = C1.red   < 10 ? C1.red  /3294.6:pow((C1.red  /255.0+0.055)/1.055,2.4);
    float G = C1.green < 10 ? C1.green/3294.6:pow((C1.green/255.0+0.055)/1.055,2.4);
    float B = C1.blue  < 10 ? C1.blue /3294.6:pow((C1.blue /255.0+0.055)/1.055,2.4);

    //Observer. = 2°, Illuminant = D65
    float X = R * 0.4124 + G * 0.3576 + B * 0.1804;
    float Y = R * 0.2126 + G * 0.7152 + B * 0.0722;
    float Z = R * 0.0193 + G * 0.1192 + B * 0.9505;

    X /= 0.9504;
    Y /= 1.0000;
    Z /= 1.0890;


    X = X > 0.008856 ? pow(X,0.333334 ) : 7.878*X + 16.0/116;
    Y = Y > 0.008856 ? pow(Y,0.333334 ) : 7.878*Y + 16.0/116;
    Z = Z > 0.008856 ? pow(Z,0.333334 ) : 7.878*Z + 16.0/116;

    L = ( 116 * Y ) - 16;
    a = 500 * ( X - Y );
    b = 200 * ( Y - Z );
}
```
  

Lab renk uzayı kullanılarak ölçülen Öklid uzaklığı iki renk arasındaki algısal benzerliği modelleyebilmektedir. Aşağıda yukarıda verilen tabloun Lab renk uzayı kullanılarak benzerliği ölçüldüğünde elde edilen sonuç verilmiştir.

<table class="spectrum boxed">
<tbody>
<tr>         
<td style="background-color: #b1224c;">0.00</td>
<td style="background-color: #a56666;">6.02</td>
<td style="background-color: #ff224c;">6.06</td>
<td style="background-color: #64224c;">6.34</td>
<td style="background-color: #8b228c;">8.50</td>
<td style="background-color: #d8afb4;">9.88</td>
<td style="background-color: #b10000;">11.70</td>
<td style="background-color: #b100e8;">15.63</td>
<td style="background-color: #fcff00;">22.61</td>
<td style="background-color: #2cda30;">25.64</td>
</tr>
</tbody>
</table>

Denklemle ilgili tüm bilinmeyenlerimizi açıkladığımıza göre kanal ağırlıklarını bulmaya başlayabiliriz. İlk yapmamız gereken imge üzerinde `K` tane gözek çifti seçerek bu çiftler arasındaki uzaklığı $\delta_{x,y}$ bulmak.

```c
void sample(BMP I, RGBA *C1, RGBA *C2, float *D, int K)
{
    int i;
    int M = I.bminfo.height;
    int N = I.bminfo.width;

    int x1,x2,y1,y2;

    for(i=0; i < K; i++) {

        x1 = rand()%N; x2 = rand()%N;
        y1 = rand()%M; y2 = rand()%M;

        C1[i] = I.pixels[x1][y1];
        C2[i] = I.pixels[x2][y2];

        D[i] = ColorDistance(C1[i],C2[i]);
    }
}
```

Yukarıda verilen fonksiyon imge üzerinde rastgele nokta çiftleri seçmekte ve bu noktadaki renkleri `C1` ve `C2` değişkenlerinde saklamakta. `D` değişkeninde ise yukarıda anlatımını ve çıkarımını yaptığımız $\delta_{x,y}$ değeri saklanmakta.

Gözekler arasındaki uzaklık bulunduktan sonra tek yapmamız gereken
imgeyi $w_r, w_g, w_b$ ağırlıklarını kullanarak gri seviyeye çevirip,
enerji fonksiyonumuzun değerini kontrol etmek. Bu çalışmaya özel
ağırlıkları bulmak için kullanılacak en hızlı yöntem deneme yanılma
yöntemidir. Ağırlıkları belirlemek için üç ağırlık değeri de sıfırdan
başlayarak 0.1 artımlarla artırılarak enerji fonksiyonunun değeri
hesaplanır. Denenmesi gerek ağırlık kırmızı kanal için 11, yeşil kanal
için 11 ve mavi kanal için 11 tanedir. Ancak  $w_r+w_g+w_b=1$
şartının da sağlanması gerektiğinden denenebilecek toplam 66 farklı
ağırlık çifti vardır. Ağırlıklardan bazıları aşağıdaki tabloda
verilmiştir.

|--- |--- |--- |--- |--- |--- |--- |--- |--- |--- |--- |--- |--- |--- |
|0.0|0.0|0.0|0.0|0.0|0.3|0.3|0.3|0.3|0.6|0.6|0.6|0.6|1.0|
|0.0|0.1|0.4|0.7|1.0|0.0|0.3|0.6|0.7|0.0|0.2|0.3|0.4|0.0|
|1.0|0.9|0.6|0.3|0.0|0.7|0.4|0.1|0.0|0.4|0.2|0.1|0.0|0.0|

Yukarıdaki tablonun W matrisinde tutulduğunu varsayarsak, ağırlıkların
aramasını yapan kodu aşağıdaki şekilde yazabiliriz.

```c
for(j=0; j < 66; j++) 
{
    wr = W[j][0];
    wg = W[j][1];
    wb = W[j][2];

    Eg = 0;

    for(i=0; i < K; i++) 
    {
        dX = wr*C1[i].red + wg*C1[i].green + wb*C1[i].blue;
        dY = wr*C2[i].red + wg*C2[i].green + wb*C2[i].blue;

        Eg += (fabs(dX-dY)-D[i])*(fabs(dX-dY)-D[i]);
    }

    if(Eg < minEg) 
    {
        minEg = Eg;
        *wR = wr;
        *wG = wg;
        *wB = wb;
    }
                
}
```

Yukarıda verilen kod bir imge için çalıştığında, imgenin doku bilgisini
tutabilecek gri seviye dönüşüm ağırlıklarını vermektedir. Bu ağırlıklar
kullanılarak gri seviye dönüşüm yapıldığında imgenin dokusu
olabildiğince korunacaktır.  
  
Yazımızı her zamanki gibi örneklerle bitirelim. Aşağıda verilen
resimlerden ilki renkli imgeyi, ikincisi klasik gri seviye dönüşüm
sonucunu ve üçüncü imge ise imge renksizleştirme işleminin sonucunu
göstermekte.  
  
![İmge Renksizleştirme Örnek][image_decolor2]
![İmge Renksizleştirme Örnek][image_decolor1]
  
Örnek olarak kullandığım resimlerin çoğu imge renksizleştirme
yayınlarında kullanılan örnek resimler. Eğer diğer yayınların
sonuçlarını görmek ve bu sonuçlarla karşılaştırmak isterseniz aşağıdaki
referanslara göz atabilirsiniz.  
  
**Referanslar**  

*  Cewu Lu, Li Xu, Jiaya Jia, "Contrast Preserving Decolorization", IEEE International Conference on Computational Photography (ICCP), 2012.
*  Cewu Lu, Li Xu, Jiaya Jia, "Contrast Preserving Decolorization with Perception-Based Quality Metrics", International Journal of Computer Vision (IJCV), 2014
*  [http://www.cse.cuhk.edu.hk/leojia/projects/color2gray/index.html](http://www.cse.cuhk.edu.hk/leojia/projects/color2gray/index.html)
*  [http://colormine.org/convert/rgb-to-lab](http://colormine.org/convert/rgb-to-lab)

[RESOURCES]: # (List of the resources used by the blog post)
[image_decolorization]: /assets/post_resources/image_decolorization/image_decolorization.png
[image_decolor1]: /assets/post_resources/image_decolorization/image_decolor1.png
[image_decolor2]: /assets/post_resources/image_decolorization/image_decolor2.png
