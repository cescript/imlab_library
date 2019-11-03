---
layout: post
title: Barkod Tespiti (Barcode Detection)
date: '2019-04-28T23:41:00.001+03:00'
author: Bahri ABACI
categories:
- Görüntü İşleme Uygulamaları
modified_time: '2019-07-14T23:06:30.883+03:00'
thumbnail: /assets/post_resources/barcode_detection/thumbnail.png
---

Barkodlar bilgisayarlar tarafından hızlı bir şekilde tanımlanabilen ve üzerinde bulunduğu nesne hakkında bilgiler taşıyan küçük etiketlerdir. Adını İngilizce' de bar (çubuk) ve code (şifreleme) kelimelerinin birleşmesinden alan barkod teknolojisi, ilk olarak 1952 yılında Norman Joseph Woodland ve Bernard Silver tarafından, sabit bir alan içerisinde yer alan kodlar (çizgiler ve renkler) yardımıyla ürünlerin otomatik olarak tanınması ihtiyacını karşılamak için önerilmiştir. Günümüzde bir boyutlu (barkod) ve iki boyutlu (karekod) olmak üzere iki ana gruba ayrılan barkodların, farklı kullanım koşullarına göre geliştirilmiş (Code 128, EAN 8, EAN 13, Data Matrix, QR, HCCB, vs) gibi onlarca çeşidi bulunmaktadır.

<!--more-->

Bu yazımızda kullanacağımız tespit yöntemi ile aralarında Code 128, EAN13 gibi günümüzde sıklıkla kullanılan bir boyutlu barkodların görüntüler üzerinden otomatik olarak tespit edilmesi hedeflenmektedir. Kullanılan yöntem büyük oranda Orazio Gallo ve Roberto Manduchi tarafından [Reading 1-D Barcodes with Mobile Phones Using Deformable Templates](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3190667/) makalesinde önerilen bir boyutlu barkod tespit algoritmasından esinlenerek oluşturulmuştur.

Bir boyutlu barkodların üzerinde bulunduğu nesneden ayırt edici en önemli özelliği üzerinde barındırdığı düz çizgilerdir. Bu çizgiler bir kenar tespiti algoritması ile vurgulandığı takdirde, barkod içeren bölgelerin yüksek miktarda dikey kenar bulundurduğu görülmektedir. Tespit edilen barkodların her zaman yatay olmayacağı da hesaba katılarak, dikey kenarların vurgulandığı imgeden, yatay kenarların vurgulandığı imge çıkarılarak, barkod karakteristik özelliğine sahip bölgelerin vurgulanması sağlanır.

$I(x,y)$ bir boyutlu barkod içeren bir imge, $(\nabla_x, \nabla_y)$ sırasıyla $x$ ve $y$ yönünde gradyan operatörünü göstermek üzere; barkod olması muhtemel bölgeler $I_e(x,y)$ şu şekilde hesaplanır: $$I_e(x,y) = \max(0, \min(255, \nabla_x I(x,y)- \nabla_y I(x,y)))$$ Bu işlem sonucunda elde edilen değerler oldukça gürültülü olduğundan, barkodların uzamsal domendeki ilişkisi de göz önüne alınarak basit bir [box blur süzgeci](http://www.cescript.com/2019/02/tumlev-imge-integral-image.html) ile $I_e(x,y)$ gürültüden arındırılarak $I_m(x,y)$ imgesi elde edilir. $I$ girdi görüntüsünden $I_m$ enerji haritasının elde edilmesinde kullanılan kod bloğu aşağıda verilmiştir.

```c
// compute energy map
return_t energy(matrix_t *in, matrix_t *out)
{
    uint32_t i,j,k;

    uint32_t block_width  = 31;
    uint32_t block_height = 11;

    // resize the output matrix
    matrix_resize(out, rows(in), cols(in), channels(in));

    // create temp matrix for the edge intensity calculation
    matrix_t *edges = matrix_create(uint8_t, rows(in), cols(in), channels(in));

    // compute the edge intensity
    for(j = 1; j < rows(in)-1; j++)
    {
        for(i = 1; i < cols(in)-1; i++)
        {
            int hEdges = abs(atui8(in, j + 1, i, 0) - atui8(in, j - 1, i, 0));
            int vEdges = abs(atui8(in, j, i + 1, 0) - atui8(in, j, i - 1, 0));

            // combine edges
            atui8(edges, j, i, 0) = clamp(vEdges - hEdges, 0, 255);
        }
    }

    // remove noise by using box blur
    box_filter(edges, block_width, block_height, out);

    // delete temp matrix
    matrix_free(&edges);
}
```
Enerji haritasının bulunmasından sonraki adımda ise süzgeçlenen imgeler [Otsu](http://www.cescript.com/2012/07/otsu-metodu-ile-adaptif-esikleme.html) yöntemi ile bulunan eşik değeri kullanılarak siyah beyaza dönüştürülerek [bağlantılı bileşen etiketleme](http://www.cescript.com/2012/09/baglantili-bilesen-etiketleme.html) için hazırlanır. Bu işlemler sonucunda elde edilen örnek imgeler aşağıda verilmiştir.

![barcode tespiti adımları][steps]

Verilen imgede; ilk sütun gri seviyeye dönüştürülmüş girdi imgelerini, ikinci sütun $I_e(x,y)$ dikey ve yatay kenar imgelerinin farkını, üçüncü sütun $I_m(x,y)$ box blur yöntemi ile süzgeçlenmiş barkod olması muhtemel bölgeleri ve son sütun ise Otsu yöntemi ile siyah beyaza çevrilmiş (ikili kodlanmış) ön işlem sonucunu göstermektedir. *Görselliğin iyileştirilmesi amacıyla ikinci ve üçüncü sütunlarda histogram eşitleme sonucu gösterilmiştir.*

Bu aşamadan sonra literatürde; [morfolojik operatörler](http://www.cescript.com/2012/08/morfolojik-goruntu-isleme.html) yardımıyla küçük bileşenlerin temizlenmesi, ayrık bileşenlerin birleştirilmesi gibi pek çok farklı işlem ile barkod bölgesinin etrafında bulunan gürültülerin temizlenmesi önerilmiştir. Yazımızda bu işlemler atlanarak, siyah beyaz dönüşümü sonrasında görüntüde bulunan en büyük alana sahip bağlantılı bileşenin barkodun bulunduğu bölge olduğu varsayılmıştır. 

<hr align="center" color="yellow" size="2" width="100%" /><span style="color: yellow;">DİKKAT: </span> 

Bu varsayım ile görüntüde her zaman en az bir barkod olduğu da dolaylı olarak kabul edilmiştir. Tespit edilen barkodların çözülme aşamasında yapılacak ilave kontroller ile tespit aşamasında bulunan yanlış tespitler, tanıma aşamasında elenebileceğinden bu varsayımın olası olumsuz etkileri göz ardı edilmiştir.

<hr align="center" color="yellow" size="2" width="100%" />

Bağlantılı bileşen etiketleme yapmak için IMLAB görüntü işleme kütüphanesinde yer alan `bwconncomp` fonksiyonu kullanılmıştır. Bu fonksiyon verilen ikili imge üzerinde bağlantılı bileşen analizi yapmakta ve tüm bağlantılı bileşenlerin imge üzerindeki koordinatlarını `vector_t` formatında döndürmektedir. Ardından tüm bağlantılı bileşen vektörleri tek tek gezilerek en uzun vektör (en büyük bağlantılı bileşen) bulunmuştur. Bulunan koordinatları içeren en küçük dikdörtgen de `point_fit_rectangle` fonksiyonu yardımı ile hesaplanmış ve bu dikdörtgen algoritmanın çıktısı olarak işaretlenmiştir.

```c
return_t find_barcode(matrix_t *img, struct point_t minAreaRect[4])
{
    matrix_t *img_gray = matrix_create(uint8_t);

    // convert input into grayscale
    rgb2gray(img, img_gray);

    // compute edge energy
    matrix_t *filter = matrix_create(uint8_t, rows(img_gray), cols(img_gray), 1);
    energy(img_gray, filter);

    // threshold energy map
    imthreshold(filter, 0.6 * imotsu(filter), filter);

    // do connected component labeling
    uint32_t numberOfComponenets = 0;
    vector_t **components = bwconncomp(filter, &numberOfComponenets);

    // find the largest connected component
    uint32_t largestPixelCount = 0;
    uint32_t largestID = 0;

    uint32_t i = 0;
    for (i = 0; i < numberOfComponenets; i++)
    {
        if (length(components[i]) > largestPixelCount)
        {
            largestPixelCount = length(components[i]);
            largestID = i;
        }
    }

    // find minimum area rectangle
    point_fit_rectangle(vdata(components[largestID], 0), length(components[largestID]), minAreaRect);

    // deallocate the unused memory
    for (i = 0; i < numberOfComponenets; i++)
    {
        vector_free(&components[i]);
    }
    free(components);
    matrix_free(&filter);
    matrix_free(&img_gray);

    return SUCCESS;
}
```

Yukarıda verilen kod parçasının örnek resimler üzerindeki çıktıları aşağıdaki resimde verilmiştir. Örneklerden de görüldüğü üzere önerilen yöntem değişik imgeler üzerinde (farklı ışık koşulları, farklı açılar, karışık arka planlar) başarılı bir şekilde tespit yapabilmektedir.

![barcode tespit sonuçları][results]

Önerilen yöntemin başarısının nümerik sonuçlarla ifadesi için Alessandro Zamberletti, Ignazio Gallo and Simone Albertini tarafından **Robust Angle Invariant 1D Barcode Detection** makalesinde önerilen ve yukarıda da bazı örnekleri kullanılan [bir boyutlu barkod veri seti](http://artelab.dista.uninsubria.it/downloads/datasets/barcode/hough_barcode_1d/hough_barcode_1d.html) bir boyutlu barkod veri seti üzerinde çalışmalar yapılmıştır.

Yapılan çalışmada veri  seti sağlayıcıları tarafından verilen maskelerden `point_fit_rectangle` fonksiyonu yardımı ile barkod bölgesini içeren en küçük dikdörtgen hesaplanmıştır. Hesaplanan bu dikdörgen ($M$) ile tespit algoritmasının çıktısı ($R$) kullanılarak, $\tau = \frac{M \cap R}{M \cup R}$ ifadesi ile kesişim oranı hesaplanmıştır. Bu oranın $\tau > 0.7$ olması durumunda tespit algoritmasının başarılı çalıştığı varsayılmıştır. Bu işlem sonucunda veri setinde yer alan $365$ test imgesinin $288$ tanesinde barkod tespiti başarılı bir şekilde yapılmıştır. Literatürde pek çok makalede de kullanılan $\tau > 0.5$ eşik değeri içinse $325$ imge başarılı olarak sınıflandırılarak $\%89$ tespit başarı oranı yakalanmıştır.

Çalışmaya ait sonuçlar ($\tau > 0.7$) ve kullanılan kaynak kodlar [Barkod Tespiti projesinin GitHub sayfasında](https://github.com/cescript/cescript_blog_barcode_detection) paylaşıma açılmıştır.

**Referanslar**
* Gallo, Orazio, and Roberto Manduchi. "Reading 1D barcodes with mobile phones using deformable templates." IEEE transactions on pattern analysis and machine intelligence 33.9 (2011): 1834-1843.
* Zamberletti, Alessandro, Ignazio Gallo, and Simone Albertini. "Robust angle invariant 1d barcode detection." 2013 2nd IAPR Asian Conference on Pattern Recognition. IEEE, 2013.
* Bodnár, Péter, and László G. Nyúl. "Barcode detection using local analysis, mathematical morphology, and clustering." Acta Cybernetica 21.1 (2013): 21-35.

[RESOURCES]: # (List of the resources used by the blog post)
[steps]: /assets/post_resources/barcode_detection/barcode_preprocess_steps.png
[results]: /assets/post_resources/barcode_detection/barcode_detection_results.png