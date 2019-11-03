---
layout: post
title: İçerik Tabanlı İmge Ölçekleme
date: '2015-07-14T21:03:00.000+03:00'
author: Bahri ABACI
categories:
- Görüntü İşleme Uygulamaları
- Veri Analizi
modified_time: '2015-09-16T12:14:27.014+03:00'
thumbnail: /assets/post_resources/content_based_image_scaling/thumbnail.png
---

Ölçekleme (görüntü boyu değiştirme) verilen bir imgenin boyutunun
istenilen boyutlara, doğrusal veya doğrusal olmayan yollarla ,yeniden
ayarlanması işlemidir. Kısa bir zaman öncesine kadar ölçekleme işlemini
kıymetli kılacak bir gereksinim ortaya çıkmamış ve çok standart bir
yöntem olan [örnekleme yöntemi](http://www.cescript.com/2013/12/perspektif-donusumu.html) ile
alınan sonuçlar yeterli görülmüştür. Ancak, görüntüleme cihazlarının
(telefon, monitör, vs.) ve ortamlarının  (facebook, instagram, vs.)
çeşitlenmesi ile düzgün olmayan örnekleme yöntemlerine ihtiyaç
duyulmuştur. Bu olayın sıklıkla karşımıza çıkan örneklerinden biri
instagram' ın kare imge şablonudur. Bu şablon nedeniyle imgeler boyutu
ne olursa olsun kare biçimine getirilmesi gerekmektedir. 

<!--more-->
  
İmgeyi doğrusal örnekleme yöntemi ile ölçeklendirme görüntüdeki
cisimlerin en-boy oranını değiştirirken, imgeyi kesme ise imgede
kalmaması istenen bazı detayların silinmesine neden olmaktadır.  
  

İçerik tabanlı imge ölçekleme (Content Aware Image Resizing) imgenin
ölçeklenmesi sırasında içeriğindeki bileşenleri önemlerine göre
koruyarak veya silerek yapılan ölçekleme işlemdir. Bu ölçekleme
doğrusal/düzgün olmadığından imge içerisinde gereksiz görülen nesneler
silinerek imge boyunun azaltılması sağlanacaktır. Bu çalışmada
gerçeklemesini yapacağımız yöntem ise 2007 yılında Siggraph
konferansında tanıtılan Seam Carving yöntemi. Bu yöntem iteratif biçimde
çalışarak, ilkin ölçeklenecek imgenin enerji haritasını bulmakta,
ardından her satırda en düşük enerjili piksel değerini silmektedir.
Böylece imge herbir iterasyonda bir sütun eksilmekte ve iterasyonun N
kez tekrarlanması ile imge boyu N kadar küçülmektedir. 

  

Yöntemin kodlamasına geçmeden önce, hızlıca sonuca bakarak yöntemin daha
anlaşılır olmasını sağlayabiliriz. Aşağıdaki resimde orjinali 281x1000
(satır x sütun) olan Canberra balon festivali fotoğraflarını instagramda
paylaşmak istediğimizi varsayalım.  
  
![İmge Ölçekleme Örnek Görüntü][rsz_test2]
  
İmge kare olmadığından fotoğrafı paylaşmak için olası çözüm resmi
281x281' luk bir şablona kırpmak olacaktır. Fotoğraf düzenleme ile arası
iyi olanlar ise resmi 281x1000 pikselden 281x281 piksele kırpmayı da
deneyebilirler. Bu yazımızda öğreneceğimiz Seam Carving kullanılmak
istenirse de yöntem resim üzerinde 719 kez çalıştırılarak 281 x 281
(1000-719) piksellik kare bir resim elde edilebilir. Yapılan üç farklı
düzenleme için elde edilen sonuçlar aşağıda verilmiştir.  
  
![İmge Ölçekleme Örnek Çıktılar][balloon_seam]

Görüntülerden de anlaşılacağı gibi doğrusal ölçekleme (ortada), imge
içerisindeki nesnelerin en-boy oranlarını değiştirdiğinden göze hoş
gelmeyen bir sonuç üretmekte. Görüntüyü kırpmak (solda) ile doğrusal
olmayan ölçeklemeden (sağda) geçirmek arasında ise ilk bakışta çok büyük
bir fark görülmemekte. Ancak resmi detaylı inceleyecek olursak, doğrusal
olmayan ölçeklemede sağ tarafta bulunan küçük balonların, bina
girişinin, sağda bulunan ağaçların, vs. korunduğunu görülecektir. Peki
gelelim bunu nasıl yaptığımıza.  
  
## Seam Carving
  
Yazının başında da belirttiğim gibi amacımız ilk olarak imge içerisinde
enerjisi (belirginliği, önemi) en yüksek olan nesneleri belirlemek.
Ardından her satır için, en düşük enerjili yolun geçtiği pikselin
silinmesi.  
  
### Enerji Matrisinin Belirlemesi

Bildiride enerji belirlenmesi için farklı yöntemler önerilse de biz bu yazıda en basit enerji çıkarma işlemini, Sobel filtrelemeyi kullanacağız. Sobel süzgeci imge kenarlarını yakalamada yaygın kullanılan bir tür gradyan filtresidir. Filtreleme önceki yazılarımızda değindiğimiz [konvolüsyon (evrişim) işleminin](http://www.cescript.com/2012/07/c-ile-konvolusyon-islemi.html) $$h = \left[\begin{smallmatrix} 1 & 2 & 1 \\ 0 & 0 & 0 \\ -1 & -2 & -1 \end{smallmatrix} \right]$$ çekirdek matrisi ile imgeye uygulanması ile gerçeklenir. Bu uygulama sonucunda görüntünün x yönündeki gradyanı ($G_{xdir}$) (değişintisi) bulunmuş olur. Aynı işlemi $$h = \left[ \begin{smallmatrix}1 & 0 & -1 \\ 2 & 0 & -2 \\ 1 & 0 &-1 \end{smallmatrix}\right]$$ çekirdek matrisi ile yaparsak ise görüntünün y yönündeki gradyanı ($G_{ydir}$) bulunmuş olur. Yazımızda kullanılan enerji matrisi görüntünün x ve y yönündeki değişintilerinin toplamının ($G_xG_y$) gri seviye kodlanması ile hesaplanmıştır.  
  
```c
double Gx[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
double Gy[9] = {1, 0, -1, 2, 0, -2, 1, 0, -1};
    
BMP Gxdir = resim_kon(I,Gx,3);
BMP Gydir = resim_kon(I,Gy,3);

BMP GxGy  = topla(Gxdir,Gydir);
```

Yukarıdaki kodun sonuçlarına adım adım bakacak olursak.  
  
![Kapadokya Balon Görüntüsü][kapadokya]
  
İlk sırada enerji hesaplaması için gönderilen resim (**I**) görülmekte. İkinci sırada **I** resminin **Gx** ile evrşimi sonucu bulunan ve x yönündeki değişinti noktalarını vurgulayan **Gxdir** görüntüsü bulunmakta. Üçüncü sırada ise **I** resminin **Gy** ile evrişimi sonucu elde edilen ve y yönündeki değişim noktalarını vurgulayan **Gydir** görüntüsü yer almakta. Dördüncü sırada yer alan imge imgenin **Gxdir** ve **Gydir** görüntülerinin piksel piksel toplamı ile elde edilen ve resmin herhangi bir yönde var olan değişimini vurgulayan **GxGy** imgesi bulunmakta. Son sırada ise çalışmada enerji yoğunluğunu kestirmek için kullanacağımız ve **GxGy**' nin gri seviye kodlanması ile elde edilen `enerji matrisi` yer almakta. Bu görüntüde dikkat edilecek olursa, balon, dağ gibi değişimlerin yüksek olduğu bölgeler beyaz (yüksek enerjili) iken gökyüzü gibi alanlar siyah (düşük enerjili) olarak işaretlenmiştir. Yani algoritmamızdan resmin küçültülmesi istendiğinde bulutları silerken, balon ve dağ piksellerini koruyacaktır.  
  
### En Düşük Enerjili Yol 

Yukarıdaki enerji haritası yardımıyla farklı teknikler kullanılarak
boyut indirgemesi yapılabilir. Bunlardan biri her satırdan, o satırın en
düşük enerjili piksel değerini silmektir. Anacak böyle bir silme işlemi
her satırda farklı sütunlardan pikseller sileceğinden görüntüde
kaymalara/bozulmalara neden olacaktır. Kaymaları engellemek için
kullanılacak bir diğer yöntem ise her seferinde en düşük enerjili sütunu
kaldırmaktır. Bu durumda ise imge içerisinde bazı önemli nesnelerin
silinebilmesi söz konusudur. Bildiride önerilen ve yukarıdaki
olumsuzlukları gideren yöntem en düşük enerjili yolu silmeyi
önermektedir. Enerjili yolu (seam), görüntünün tepesinden başlayıp
tabanında son bulan ve her satırdan tek bir kez geçen eğrilere verilen
isimdir. Amacımız imge boyunu azaltırken oluşacak bilgi kaybını en aza
indirmek olduğundan,  olası tüm yollar içerisinden en düşük enerjili
yolun bulunarak silinmesi gerekmektedir.  
  
Ancak arama uzayımız imgenin genişliği ile doğrusal, yüksekliği ile
üstel büyüdüğünden, olası tüm yolların bulunup en düşük enerjili eğrinin
seçilmesi verimli olmamaktadır. Amacımız, bu yollar içerisinden en düşük
enerjili olanı bulmak olduğundan, dinamik programlamayı kullanarak
görüntü boyu ile doğrusal  karmaşıklıkta problemi çözebiliriz. Algoritma
uzun ve karmaşık olduğundan detayları bir sonraki yazıya bırakıyorum.
Şimdilik bulunan bu yolun path dizisinde saklandığını ve bu dizinin y'
inci elemanı, eğrinin x koordinatını  (x = path\[y\]) içerdiğini
bilmemiz yeterli.  
  
Aşağıda Canberra fotoğrafı üzerindeki en düşük enerjili 100 eğri/yol
gösterilmiştir. Eğriler silinecek piksel değerlerini gösterdiğinden
dikkatli bakılırsa 100 piksel silinse dahi resmin önemli figürleri olan
balon, bina gibi noktalarda kayda değer bir eksilme olmayacaktır.  
  
![Kapadokya Balon Görüntüsü Kare][seam_carving_balloon]
  
En düşük enerjili yolun belirlenmesinin ardından resimde bu yolun
geçtiği tüm piksel değerleri silinecek (kaldırılacaktır). Böylelikle
resim bir piksel küçülmüş olacaktır.  
  
```c
BMP YoluKaldir(BMP I,int *path) {

    int M = I.bminfo.height;
    int N = I.bminfo.width;

    int i,j,maxEn=0,t,ii;

    BMP im = yenim_bmp(N-1,M);

    for(j=0;j < M;j++) 
    {

        for(i=0;i < path[j]; i++)
        {
            im.pixels[i][j].red   = I.pixels[i][j].red;
            im.pixels[i][j].green = I.pixels[i][j].green;
            im.pixels[i][j].blue  = I.pixels[i][j].blue;
        }
        for(i=path[j]+1;i < N; i++) 
        {
            im.pixels[i-1][j].red   = I.pixels[i][j].red;
            im.pixels[i-1][j].green = I.pixels[i][j].green;
            im.pixels[i-1][j].blue  = I.pixels[i][j].blue;
        }

    }
return im;
} 
```
  
Görüldüğü üzere kod o kadar da karmaşık değil. İlk olarak verilen
imgenin genişliğinin bir azı genişlikte bir imge yaratıyoruz. Ardından
her satır için, silinecek olan piksele kadar tüm pikselleri yeni imgeye
kopyalıyoruz. **path\[j\]**: j. satır için silinecek olan sütunu
gösterdiğinden bu değeri atlayarak kopyalama işlemine devam ediyoruz.  
  
İmgenin istenilen boya gelmesi için enerji hesaplama, en düşük enerjili
yol bulma ve silme işlemlerini iteratif olarak istenilen kadar
tekrarlıyoruz. (Kod içerisinde mavi ile renklendirilen kısımlar bir
sonraki yazıya bıraktığımız en düşük enerjili yolu bulma kısmı.)  
  
```c
while (k < K) 
{
    int M = I.bminfo.height;
    int N = I.bminfo.width;

    BMP Gxdir = resim_kon(I,Gx,3);
    BMP Gydir = resim_kon(I,Gy,3);

    BMP GxGy  = topla(Gxdir,Gydir);

    EnerjiHesapla(Enerji,GxGy);

    // Bir sonraki yazı ile anlaşılabilir.
    // Dinamik programlama ile enerji hesapla
    minI    = DinamikEnerji(Enerji,ToplamEnerji, M, N);
    MinEnerjiYolu(ToplamEnerji, path, minI, M, N );

    I = YoluKaldir(I,path);

    k++;
} 
```
  
Yazımızı örneklerle bitirme safhasına geldik sanırım. Test için farklı
boyutlarda bulunan geniş açılı resimleri seam carving ve düzgün
örnekleme ile kare haline getirmeye çalıştım.  
  
![İmge Ölçekleme Örnek][samples]
  
Yazıdan da fark edebileceğiniz bir gerçek, seam carving tüm fotoğraflar
için iyi sonuç üretmeyeceğidir. Hangi tip fotoğrafların seam carving,
hangi tip fotoğrafların ise kırpma işlemine ihtiyacı olduğunu düşünme
kısmını size bırakayım. Zaten sonuçlara dikkatli bakarsanız doğru kararı
verip vermediğinizi anlayacaksınızdır.

**Referanslar**
* Avidan, Shai, and Ariel Shamir. "Seam carving for content-aware image resizing." ACM Transactions on graphics (TOG). Vol. 26. No. 3. ACM, 2007.

[RESOURCES]: # (List of the resources used by the blog post)
[rsz_test2]: /assets/post_resources/content_based_image_scaling/rsz_test2.png
[balloon_seam]: /assets/post_resources/content_based_image_scaling/balloon_seam.png
[kapadokya]: /assets/post_resources/content_based_image_scaling/kapadokya.png
[seam_carving_balloon]: /assets/post_resources/content_based_image_scaling/seam_carving_balloon.png
[samples]: /assets/post_resources/content_based_image_scaling/ornekler.png
