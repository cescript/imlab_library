---
layout: post
title: Barkod Tespiti (Barcode Detection)
date: '2019-04-28T23:41:00.001+03:00'
author: Bahri ABACI
tags:
- Görüntü işleme
- Barkod tespiti
modified_time: '2019-07-14T23:06:30.883+03:00'
thumbnail: https://3.bp.blogspot.com/-kpuUF6Jsr_I/XMSRwov4csI/AAAAAAAAByI/OOU8Yo6_DC4FAsW3zVq83ZrmqyPusRlAgCEwYBhgL/s72-c/title_image.png
blogger_id: tag:blogger.com,1999:blog-680077327709981593.post-2597326432856321343
blogger_orig_url: http://www.cescript.com/2019/04/barkod-tespiti-barcode-detection.html
content_markdown: 'content/barcode_detection/README.md'
---

{% capture post_content %}{% include_relative {{ page.content_markdown |  }} %}{% endcapture %}

{% assign post_content_split = post_content | split: site.excerpt_separator %}
{{post_content_split[0] | markdownify}}
<!--more-->
{{post_content_split[1] | markdownify}}