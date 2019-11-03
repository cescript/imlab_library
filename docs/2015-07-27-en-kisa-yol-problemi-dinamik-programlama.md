---
layout: post
title: En Kısa Yol Problemi (Dinamik Programlama)
date: '2015-07-27T22:41:00.004+03:00'
author: Bahri ABACI
categories:
- Makine Öğrenmesi
- Hızlı Algoritmalar
modified_time: '2015-09-16T12:12:53.170+03:00'
thumbnail: /assets/post_resources/dynamic_programming/thumbnail.png
---

En kısa yol bulma problemi günlük hayatta karşımıza sıklıkla çıkan bir
problemdir. Adından da anlaşıldığı üzere, problemin amacı herhangi bir
noktadan harekete başlayıp, belirli noktaları ziyaret etmek şartı ile
varış noktasına en az enerji / en hızlı zaman / en az masrafla gitmek
için seçilmesi gereken yolu bulmaktır. Problemin en temel kullanım
alanlarından biri araç navigasyon sistemleri gibi görünse de evlerimizde
kullanılan router (yönlendirici), robot navigasyonu, telemarket
operatörlerinin zamanlanması gibi çok farklı alanlarda kullanılmaktadır.
Algoritmanın bu kadar yaygın bir şekilde en iyileme gerektiren işlerde
kullanılması, problemin çözümü kadar çözümün verimliliğini de önemli
kılmaktadır.  
  
<!--more-->

Örneğin bir router kullanıcıyı en iyi şekilde internete ulaştırmaya
çalışırken, en iyi yolu seçme aşamasında saniyeler kaybediyorsa,
kullanıcıyı en hızlı yoldan dahi internetle buluştursa kullanıcının
hissettiği kayda değer bir hızlanma olmayacaktır. Problemi akla
gelebilecek en basit çözümüyle  çözmeye çalışalım.  
  
Sanırım problemin akla gelen ilk çözümü olası tüm yolların denenmesi ve
en düşük enerjili yolun seçilmesidir. Bu yöntem doğruluk açısından
hiçbir tehlike içermese de algoritmanın çalışma süresi oldukça yavaş
olacaktır. [İçerik tabanlı imge ölçekleme](http://www.cescript.com/2015/07/icerik-tabanli-imge-olcekleme.html)
yazısında karşımıza çıkan problemi ele alalım. Örneğimizde bir enerji
matrisinin ilk satırından başlayıp her satırdan bir kez geçerek son
satırında son bulan, en az enerjili yolu arıyoruz.  
  
Bu yolun sürekli olduğunu bildiğimizden ilk satırda herhangi bir
sütundan başlayarak, her adımda bir piksel alt-sağa, bir piksel alt-sola
veya doğrudan bir piksel alta hareket ederek resmin son satırına
vardığımızda aranılan özellikte bir yol elde etmiş olacağız. Örnek
matrisimizin boyunun MxN olduğunu düşünürsek, olası tüm yolların sayısı
yaklaşık olarak Nx3^M (her satır için üç: alt-sağ, alt, alt-sol seçenek
var) tanedir. Burada M' nin küçük değerleri için yol hesabı yapmak
mümkünken, M=20 için dahi (~9 milyar yol) işlem yapmak dakikalar
alacaktır. Peki problemim çözümünü nasıl hızlandırılabilir?  
  
Sorunun cevabı aslında problemin isminde de vurgulanan en kısa
sıfatında. Aradığımız olası tüm yolların enerjisi olmadığından, en kısa
yolu, uzun olacağı önceden belli yolları eleyerek adım adım tarama
yöntemiyle bulabiliriz. Problemin çözümü için farklı yöntemler önerilse
de bu yazımızda anlatacağımız Dinamik Programlama bu mantıkla
çalışmakta.  
  
Bir örnek üzerinden giderek dinamik programlama mantığını anlamaya
çalışalım. Aşağıda görülen grafikte yuvarlak noktaları (düğümleri)
matrisin elemanları, içlerindeki değerleri ise o elemana gelebilmek için
gereken enerji olarak düşünelim. Amacımız en az enerjiyi harcayarak A
düğümünden F düğümüne ulaşmak.

![En Kısa Yol Problemi][graph]  

İlk olarak rastgele bir yol seçip enerji hesabı yapalım;
A->B1->C2->D2->E1->F yolu için harcanan enerji A' dan B1'
e 1, B1' den C2' ye 7, C2' den D2' ye 7, D2' den E1' e 4 ve E2' den F' e
0 olmak üzere toplam 1+7+7+4+0 = 19 olacaktır.

Aradığımız yol en az enerjili yol olduğundan, her adımda en az enerji
gerektirecek yolu seçerek de bir sonuca ulaşabiliriz. Bu durumda A' dan
B1' e 1, B1' den C1' e 5, C1' den D1' e 6, D1' den E2' e 3 ve E2' den F'
ye 0 olmak üzere 1+5+6+3+0 = 15 olacaktır. Peki bu yol en düşük enerjili
yol mudur?


Cevap tabi ki **Hayır**! Çünkü graf üzerinde
A->B3->C3->D3->E2->F yolunu izleyerek, bitişe 13 toplam
enerji ile ulaşabiliyoruz. Peki bu yolu nasıl bulabiliriz. Dinamik
programlama, en az enerjili yolu bulmak için ileri doğru hesaplama
yerine ileri-geri doğru hesaplama yapmayı öneriyor. Algoritma ileri
yönlü hesaplamada her bir düğüme ulaşılabilecek en kısa yolu (**E**)
bulmaya çalışıyor. A düğümünden B düğümlerine herhangi bir masraf
olmadığından Eb1=1, Eb2=3 ve Eb3=2 alarak hesaplamaya C düğümlerinden
başlayalım.  
  
C1 düğümüne ulaşılabilecek en uygun yol C1 düğümüne gelen yollardan en
az enerjiye sahip olanıdır. Yani Ec1=min(Eb1,Eb2)+C1=1+5=6,
Ec2=min(Eb1,Eb2,Eb3)+C2=1+7=8 ve Ec3=min(Eb2,Eb3)+C3=2+6=8 bulunur. Yani
C1 düğümüne ulaşmak için kullanılabilecek yolların en verimlisi
A->B1->C1 yoludur ve gereken enerji miktarı 6 dır. Dikkat edilirse
C1 düğümüne ulaşmanın alternatif bir yolu olan A->B2->C1 yolu daha
fazla hesaplamaya katılamayacak.  
  
Aynı hesaplamayı D düğümleri için de tekrarlayacak olursak;
Ed1=min(Ec1,Ec2)+D1=6+6=12, Ed2=min(Ec1,Ec2,Ec3)+D2=6+7=13 ve
 Ed3=min(Ec2,Ec3)+D3=8+2=10 bulunur. Hesaplamayı E düğümleri için de
yaparsak; Ee1=min(Ed1,Ed2)+E1=12+4=16,
Ee2=min(Ed1,Ed2,Ed3)+E2=10+3=13 ve  Ed3=min(Ed2,Ed3)+E3=10+5=15
bulunur.  
  
E düğümü ile F arasında herhangi bir ağırlık bulunmadığından hesaplamayı
burada tamamlayabiliriz. Elde ettiğimiz sonuçlar A düğümünden başlayarak
E1 düğümüne 16, E2 düğümüne 13 ve E3 düğümüne 15 enerji ile
gidebildiğimiz gösteriyor. Aradığımız F düğümüne ulaşan en kısa yol
olduğundan bu yolun 13 enerjiye sahip olduğunu biliyoruz. Peki bu yolu
nasıl bulacağız?  
  
Bu sorunun cevabı da algoritmanın ikinci adımı olan geri yönlü taramada.
Öncelikle ileri yönlü tarama sonucu elde ettiğimiz E enerji matrisini
grafik üzerinde gösterelim.  
  
![En Kısa Yol Problemi][graph_shortest_path]  

Çözümü verecek yolu bulmak için en düşük enerjili son düğümden
başlayarak, her seferinde en düşük enerjili bir önceki düğümü seçerek
geriye doğru  ilerliyor ve her adımda hangi düğüm üzerinde geçtiğimizi
yol listemize ekliyoruz. Yukarıda görseli verilen örnek için E2
düğümünden başlayarak, E2 den gidilecek en düşük enerjili yol D3
olduğundan D3 düğümüne geliyoruz. D3 düğümünde benzer bir karşılaştırma
yaparak C2 ve C3 düğümlerininden birine (Enerjileri eşit, ikisinden biri
ratsgele seçilebilir, biz C2 ile devam edeceğiz) geliyoruz. C2
düğümündense en küçük enerjiye sahip olan B1 düğümüne geliyoruz. Bu
işlem sonucunda toplamda seyrettiğimiz yolu
A->B1->C2->D3->E2->F ve enerjisini 13 olarak buluyoruz.

  
Gelelim kodlama kısmına, algoritmanın iki aşamadan oluştuğunu biliyoruz.
İlk aşamada ileri yönlü bir tarama yapılacak ve her satır için bir üst
satırdaki üç enerji değerinden en küçüğü o hücrenin enerji değerine
eklenecek.  

```c
int DinamikEnerji(int *Enerji,int *TE, int M, int N) 
{
    int kts = 0,i,j,minI=0;

    for(i=0;i < N; i++) { ToplamEnerji[i] = Enerji[i]; }

    for(j=1;j < M ;j++) 
    {
        kts = N*j;

        for(i=0;i < n; i < N-1)
                TE[i+kts]   = Enerji[i+kts] + min3(TE[i+kts-N-1],TE[i+kts-N],TE[i+kts-N+1]);
        else if(i==0)
                TE[i+kts]   = Enerji[i+kts] + min3(1e16,TE[i+kts-N],TE[i+kts-N+1]);
        else if(i==(N-1))
                TE[i+kts]   = Enerji[i+kts] + min3(TE[i+kts-N+1],TE[i+kts-N],1e16);
                        
        }
    }
    for(i=0;i < N; i++) 
    {
        if (TE[kts+i] < TE[kts+minI]) { minI = i; }
    }
    return minI;
}
```
  
Dikkat edilecek olursa kod içerisinde yapılan işlemlerde bundan ibaret.
Yazıda E ile tanımladığımız toplam enerji kod içerisinde TE; B,C,D,E ile
gösterdiğimiz düğüm enerjileri ise Enerji matrisi içerinde saklanmakta.
Bu fonksiyon ile ihtiyacımız olan TE ve en küçük enrjili son düğüm
bilgisine ulaşıyoruz. En düşük enerjili yolu bulmak içinse aşağıdaki
geri tarama yapan kodu kullanmamız gerekiyor.  

```c
int* MinEnerjiYolu(int *TE, int *path, int minI, int M, int N) 
{
    path[M-1] = minI;

    int a,b,c,j;
    int idx = -1;

    for (j = M-2; j >= 0; j--) 
    {
        a = TE[j*N+path[j+1]-1]; //bir üst, sol
        b = TE[j*N+path[j+1]  ]; //bir üst
        c = TE[j*N+path[j+1]+1]; //bir üst, sağ

        idx = -1;

        if (a > b) { a = b; idx++; }
        if (a > c) { a = c; idx++; }

        if (path[j+1]+idx < 1  ) { b < c ? idx=0 :idx=1; }
        if (path[j+1]+idx > N-1) { a < b ? idx=-1:idx=0; }

        path[j] = path[j+1]+idx;
    }

    return path;
}
```
  
Burada da yukarıda anlatılan işlemler dışında bir işlem bulunmamakta.
Kod içerisinde yapılan if kontrolleri komşu aramalarında imge
sınırlarının dışına çıkılmaması için konulan bir kontrolü içermekte.  
  
Algoritma sunulan hali ile girişte bahsedilen tüm problemlere
uygulanabilir durumda (Enerji matrisinin uygun şekilde doldurulması
ile). Algoritma ve kullanımı hakkında daha farklı bilgiler edinmek
isterseniz [şu
bağlantıya](http://ismailari.com/blog/dinamik-programlamaya-giris/) göz
atabilirsiniz.

**Referanslar**
* Avidan, Shai, and Ariel Shamir. "Seam carving for content-aware image resizing." ACM Transactions on graphics (TOG). Vol. 26. No. 3. ACM, 2007.

* Efros, Alexei A., and William T. Freeman. "Image quilting for texture synthesis and transfer." Proceedings of the 28th annual conference on Computer graphics and interactive techniques. ACM, 2001.

[RESOURCES]: # (List of the resources used by the blog post)
[graph]: /assets/post_resources/dynamic_programming/graf.png
[graph_shortest_distance]: /assets/post_resources/dynamic_programming/graf_kisa_yol.png
[integral_image_sample2]: /assets/post_resources/dynamic_programming/istanbul_bogazi.png
