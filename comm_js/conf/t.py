import json


with open('./comm_pre_conf.json','rb') as fp:
    datas = json.load(fp)
if datas['js_event_api'] == 'yes':
    print 'sdsdsd'
