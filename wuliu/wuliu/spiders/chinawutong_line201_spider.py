#-*- coding: UTF-8 -*-   
import sys
reload(sys)

sys.setdefaultencoding('utf-8') 
#----->>决文件里面不能写汉字的编码问题

from scrapy.selector import HtmlXPathSelector
from scrapy.contrib.linkextractors.sgml import SgmlLinkExtractor
from scrapy.http import Request
from scrapy.contrib.spiders import CrawlSpider, Rule
import time
from wuliu.items import WuTongCarLineItem

import re


class ChinaWuTongLine201Spider(CrawlSpider):
    name = 'chinawutong_line201'

    allowed_domains = ['chinawutong.com']
    start_urls=[]
    #for no in range(1,5000000):
    #    start_urls.append('http://www.chinawutong.com/201/%d.html'%no)
    
    #start_urls = ['http://www.chinawutong.com/201/4073584.html']
    start_urls = ['http://www.chinawutong.com/201/17722.html']
    #rules = (
            #Rule(SgmlLinkExtractor(allow=r'/202/[\d]+\.html'), callback='parse_item', follow=True),
    #)

    def parse(self, response):
        items = []
        item = WuTongCarLineItem()
            
        hxs = HtmlXPathSelector(response)

        item['url'] = response.url
        #item['ruku_time'] = int(time.time())  
 
        #pub_name = hxs.select('.//*[@id="UpLayer"]/ul/li[2]/p[2]/text()').extract()
        #if len(pub_name) == 0:
             #item['pub_name'] = ''
        #else:
            #m = re.match(u"([\u4e00-\u9fa5]+)：([\u4e00-\u9fa5]+)", pub_name[0].decode("utf-8"))
            #item['pub_name'] = m.group(2)            
             
        start_place    = hxs.select('.//*[@id="ctl00_cphView_fvMainLine1"]/tr[2]/td[2]/a/text()').extract()
        if len(start_place) == 0:
             item['start_place'] = ''
        else:
             item['start_place'] = start_place[0].strip()
             
        contact_name   = hxs.select('.//*[@id="ctl00_cphView_fvMainLine1"]/tr[3]/td[2]/text()').extract()
        if len(contact_name) == 0:
             item['contact_name'] = ''
        else:
             item['contact_name'] = contact_name[0].strip()
             
        tel = hxs.select('.//*[@id="ctl00_cphView_fvMainLine1"]/tr[4]/td[2]/text()').extract()
        if len(tel) == 0:
             item['tel'] = ''
        else:
             item['tel'] = tel[0].strip()
             
        phone_contact = hxs.select('.//*[@id="ctl00_cphView_fvMainLine1"]/tr[5]/td[2]/text()').extract()
        if len(phone_contact) == 0:
             item['phone_contact'] = ''
        else:
             item['phone_contact'] = phone_contact[0].strip()
             
        im     = hxs.select('.//*[@id="ctl00_cphView_fvMainLine1"]/tr[7]/td[2]/text()').extract()
        if len(im) == 0:
             item['im'] = ''
        else:
             item['im'] = im[0].strip()
             
        addr   = hxs.select('.//*[@id="ctl00_cphView_fvMainLine1"]/tr[8]/td[2]/text()').extract()
        if len(addr) == 0:
             item['addr'] = ''
        else:
             item['addr'] = addr[0].strip()
             

        #to_place      = hxs.select('.//*[@id="ctl00_cphView_fvMainLine2"]/tr[2]/td[2]/a/text()').extract()
        #if len(to_place) == 0:
             #item['to_place'] = ''
        #else:
             #item['to_place'] = to_place[0].strip()
             
        #to_name    = hxs.select('.//*[@id="ctl00_cphView_fvMainLine2"]/tr[3]/td[2]/text()').extract()
        #if len(to_name) == 0:
             #item['to_name'] = ''
        #else:
             #item['to_name'] = to_name[0].strip()
             
        #to_hphone   = hxs.select('.//*[@id="ctl00_cphView_fvMainLine2"]/tr[4]/td[2]/text()').extract()
        #if len(to_hphone) == 0:
             #item['to_hphone'] = ''
        #else:
             #item['to_hphone'] = to_hphone[0].strip()
             
        #to_mphone   = hxs.select('.//*[@id="ctl00_cphView_fvMainLine2"]/tr[5]/td[2]/text()').extract()
        #if len(to_mphone) == 0:
             #item['to_mphone'] = ''
        #else:
             #item['to_mphone'] = to_mphone[0].strip()
             
        #to_im       = hxs.select('.//*[@id="ctl00_cphView_fvMainLine2"]/tr[7]/td[2]/text()').extract()
        #if len(to_im) == 0:
             #item['to_im'] = ''
        #else:
             #item['to_im'] = to_im[0].strip()
             
        #to_addr     = hxs.select('.//*[@id="ctl00_cphView_fvMainLine2"]/tr[8]/td[2]/text()').extract()
        #if len(to_addr) == 0:
             #item['to_addr'] = ''
        #else:
             #item['to_addr'] = to_addr[0].strip()             
        
        company_name = hxs.select('.//*[@id="aspnetForm"]/div[11]/div[1]/div[2]/div[2]/div[2]/span[1]/a/text()').extract()
        if len(company_name) == 0:
             item['company_name'] = ''
        else:
             item['company_name'] = company_name[0].strip()
             
        remark    = hxs.select('.//*[@id="aspnetForm"]/div[11]/div[1]/div[4]/div[2]//text()').extract()
        if len(remark) == 0:
             item['remark'] = ''
        else:
             item['remark'] = "".join(remark).strip()
        
        item['trans_type']   = "货运"    
        item['specia_lines'] = 1
        
        items.append(item)

        return items

