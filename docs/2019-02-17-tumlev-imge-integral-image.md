---
layout: post
title: Tümlev İmge (Integral Image)
date: '2019-02-17T22:27:00.000+03:00'
author: Bahri ABACI
categories:
- Görüntü İşleme
- Hızlı Algoritmalar
modified_time: '2019-07-14T23:08:12.843+03:00'
thumbnail: /assets/post_resources/integral_image/thumbnail.png
---

Tümlev imge (integral image) görüntüde belirli bir dikdörtgenin altında kalan piksellerin toplamının hızlı bir şekilde hesaplanması için geliştirilmiş bir yöntemdir. Yöntem ilk olarak 1984 yılında Franklin C. Crow tarafından doku haritalama (texture mapping) algoritmasında kullanılmak üzere önerilse de en yaygın kullanımına 2001 yılında Paul Viola ve Michael Jones tarafından önerilen nesne tespiti algoritması ile ulaşmıştır. Günümüzde tümlev imge, işlem gücü düşük olan sistemlerde veya evrişim çekirdeğinin çok büyük olmasından dolayı yavaş çalışan süzgeçlerin hızlandırılmasında sıklıkla başvurulan bir yöntem olmuştur.

<!--more-->

Bu yazımızda tümlev imge yönteminin çalışma mantığına ve pratikte ne gibi kazançlar sağladığına değineceğiz. 

Tümlev imgede her bir piksel $(0,0)$ noktasından kendisine kadar olan tüm piksellerin toplamını göstermektedir. $M\times N$ boyutunda bir imgenin tümlev imgesinin hesaplanması için $M\times N$ boyutunda belleğe ihtiyaç duyulmaktadır. Aşağıdaki resimde sol tarafta örnek bir $3\times 3$ imge verilmiş ve bu imgenin tümlevi imgesi hesaplanmıştır.

![tümlev imge][integral_image_colored]

Tümlev imgede ($T$) turuncu kare ile seçili piksel değeri $T(0,1)$, orjinal imgede $I(0,0)+I(0,1)$ piksel değerlerinin toplamı ile hesaplanmıştır. Benzer şekilde koyu yeşil ile gösterilen piksel $T(2,1)$, orjinal imgede $I(0,0)+I(0,1)+I(1,0)+I(1,1,)+I(2,0)+I(2,1)$ işlemi ile bulunmuştur. Daha matematiksel bir ifade ile tümlev imge aşağıdaki formül ile hesaplanmaktadır.

$$T(x,y) = \sum_{i=0}^{x}\sum_{j=0}^{y}I(i,j)$$
Peki bu hesaplama bize ne kazandırmaktadır? Bu sorunun yanıtını sağ tarafta yer alan imgeyi inceleyerek bulabiliriz. Bu kez orjinal imgede kırmızı ile seçili piksellerin ($I(1:2,1:2)$) toplamını bulmaya çalıştığımızı düşünelim. Resimden yola çıkarak; kırmızı bölgenin altında kalan piksellerin toplamının, (yeşil + sarı bölgenin toplamı) - (mavi + mor bölgenin toplamı) olarak ifade edebileceğimizi görmekteyiz. Sol tarafta yaptığımız açıklamalara paralel olarak da; yeşil bölgenin toplamı = $T(2,2)$, sarı bölgenin toplamı = $T(0,0)$, mavi bölgenin toplamı = $T(0,2)$ ve mor bölgenin toplamı = $T(2,0)$ olduğunu bildiğimizden tümlev imge yardımı ile bu işlem $\sum_{i=1}^{2}\sum_{j=1}^{2}I(i,j)=45+1-6-12=28$ olarak hesaplanır. Daha genel bir durum için bu işlem aşağıdaki şekilde ifade edilebilir.

$$\sum_{i=x_0}^{x_1}\sum_{j=y_0}^{y_1}I(i,j) = T(x_1,y_1)+T(x_0-1,y_0-1)-T(x_1,y_0-1)-T(x_0-1,y_1)$$

Bu formül çok açık olmasa da aslında tümlev imge yaklaşımının ne kadar hızlı olabileceğini ve neden sıklıkla tercih edildiğini çok iyi açıklıyor. Eşitlikte sol taraf yazılımcı gözüyle incelenirse iki tane for döngüsü kullanıldığı ve bu döngülerin boyutunun $S_x=x_1-x_0+1$ ve $S_y=y_1-y_0+1$ olduğu görülmektedir. Bu ifade $O(S_xS_y)$ karmaşıklığına sahiptir. Ancak tümlev imge yardımı ile aynı işlemin sadece dört piksele erişim, $O(1)$ karmaşıklıkla yapılabildiği görülmektedir. Bu da işlem performansı açısından aynı işlemin $S_xS_y$ kat daha hızlı yapılabileceği anlamına gelmektedir.
Benzer şekilde $k\times k$ boyutlu bir evrişim çekirdeği ile $M\times N$ boyutlu bir imge evrişim yaklaşımı ile filtrelenmek istendiğinde $O(MNk^2)$ karmaşıklığıa sahip olacakken bu işlem tümlev imge ile $O(MN)$ karmaşıklığında yapılabilmektedir. Toplam işlem yükü özellikle $k > 3$ gibi değerler için $O(MNk^2)$ ile kıyaslandığında oldukça efektif olduğundan tümlev imge kullanımı pratikte oldukça büyük avantajlar sağlamaktadır. Tabi tümlev imgeyi hesaplamanın bize hiç bir maliyeti olmadığını düşünürsek!


Ne mutlu ki tümlev imge imge üzerinden tek bir geçiş $O(MN)$ ile hesaplanabilmektedir. Bu işlemi yapabilmek için tümlev imge hesaplaması ayrı bir bellekte $T$ tutulur ve $x,y$ noktasındaki tümlev imge değerini hesaplamak için, imgenin $x,y$ noktasındaki değerine ek olarak, tümlev imgenin bir soldaki, bir üstteki ve bir sol üstteki değerleri de kullanılır. Matematiksel olarak tümlev imge aşağıdaki formül ile hesaplanır:
$$T(x,y) = I(x,y) + T(x,y-1) + T(x-1,y) + T(x-1,y-1)$$

Tümlev imge hesaplamak için IMLAB görüntü işleme kütüphanesi kullanılarak aşağıdaki fonksiyon yazılmıştır. Fonksiyon, verilen bir girdi imgesi $I$ için imge üzerinden tek bir geçiş ile tümlev imgeyi $T$ hesaplamaktadır.

```c
void compute_integral_image(matrix_t *I, matrix_t *T)
{
    uint32_t x, y;

    // handle the x=0,y=0 case
    atf(T, 0, 0) = atui8(I, 0, 0);
    
    /* compute the table completely */
    for (y = 1; y < height(I); y++)
    {
        // handle the zero case
        atf(T, y, 0) = atui8(I, y, 0);

        // do the calculation for x > 0
        for (x = 1; x < width(I); x++)
        {
            atf(T, y, x) = atui8(I, y, x) + atf(T, y, x - 1) + atf(T, y - 1, x) - atf(T, y - 1, x - 1);
        }
    }
}
```
Verilen kodda $x-1$ ve $y-1$ ifadeleri $x=0$ ve $y=0$ durumlarında negatif olacağından, matematiksel ifadeye uygun olarak $$T(x,y)=\begin{cases}0,&x< 0 \lor y< 0 \T(x,y),&otherwise\end{cases}$$ kabul edilmiştir.

Çalışmada tümlev imge kullanımına örnek olarak **box blur** süzegecinin gerçeklenmesi yapılmıştır. Bu süzgeç tamamı birlerden oluşan basit bir görüntü bulanıklaştırma süzgecidir. Evrişimin mantığı gereği tamamı birlerden oluşan bir çekirdek ile yapılan evrişim işlemi, çekirdeğin altında kalan alanın toplamına karşılık gelmektedir. Bu nedenle bu süzgecin tümlev imge yardımı ile gerçeklenmesi mümkün olmaktadır. Tümlev imge yardımı ile **box blur** süzegecinin gerçeklenmesi için IMLAB görüntü işleme kütüphanesi kullanılarak aşağıdaki kod yazılmıştır.

```c
int main(int argc, char *argv[]) 
{
    int k,m,n;

    // maximum half filter size
    int hfilterSize = 20;

    // read the source image and its mask
    matrix_t *img = imread("../data//ev.bmp");
    matrix_t *mask = imread("../data//ev_mask.bmp");

    // allocate space for filtered and integral images
    matrix_t *filtered = matrix_create(uint8_t, rows(img), cols(img), 3);
    matrix_t *integral = matrix_create(float, rows(img), cols(img), 3);

    // compute the integral image
    compute_integral_image(img, integral);

    // run over the image and compute the box blur
    for (n = hfilterSize; n < height(img) - hfilterSize; n++)
    {
        for (m = hfilterSize; m < width(img) - hfilterSize; m++)
        {
            // estimate the filter size wrt mask value
            int filterSize = map(atui8(mask, n, m, 0), 0,255, 1,hfilterSize);

            // pre calculate the normalizer for the box
            float areaInv = 1.0f / ((2 * filterSize + 1) * (2 * filterSize + 1));

            // filter the channels seperately
            for (k = 0; k < channels(img); k++)
            {
                atui8(filtered, n, m, k) = clamp(areaInv * get_integral(integral, m - filterSize, n - filterSize, m + filterSize, n + filterSize, k), 0, 255);
            }
        }
    }

    // write the filtered image
    imwrite(filtered, "filtered.bmp");

    return 0;
}
```

Kodda bulanıklaştırma işlemini daha karmaşık hale getirmek ve ilginç sonuçlar elde edebilmek için süzgecin büyüklüğü maske adı verilen ikinci bir imge ile kontrol edilebilir yapılmıştır. Bu sayede resmin bazı bölgeleri bulanık yapılırken bazı bölgelerinin olduğu gibi kalabilmesi sağlanmıştır.
Kodun farklı maske ve imgelerle çalıştırılması sonucu elde edilen bazı örneklerde aşağıda verilmiştir.

![tümlev imge filtre uygulaması][integral_image_sample1]
![tümlev imge filtre uygulaması][integral_image_sample2]

Çalışmaya ait ilave dosya ve belgeler de [https://github.com/cescript/cescript_blog_integral_image](https://github.com/cescript/cescript_blog_integral_image) github sayfasında paylaşılmıştır. Çalışmada kullanılan IMLAB kütüphanesi hakkında daha detaylı bilgi almak isterseniz de [http://www.cescript.com/2018/12/imlab-goruntu-isleme-kutuphanesi.html](http://www.cescript.com/2018/12/imlab-goruntu-isleme-kutuphanesi.html) yazımıza göz atabilirsiniz.

**Referanslar**
* Crow, Franklin C. "Summed-area tables for texture mapping." ACM SIGGRAPH computer graphics. Vol. 18. No. 3. ACM, 1984.

* Viola, Paul, and Michael Jones. "Rapid object detection using a boosted cascade of simple features." Computer Vision and Pattern Recognition, 2001. CVPR 2001. Proceedings of the 2001 IEEE Computer Society Conference on. Vol. 1. IEEE, 2001.

[RESOURCES]: # (List of the resources used by the blog post)
[integral_image_colored]: /assets/post_resources/integral_image/summed_arear_table_rects_with_colors.png
[integral_image_sample1]: /assets/post_resources/integral_image/ev_box_blur.png
[integral_image_sample2]: /assets/post_resources/integral_image/istanbul_bogazi.png