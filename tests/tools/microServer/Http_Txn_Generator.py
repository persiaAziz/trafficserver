import json
import argparse
import os
import sys
import random
count = 1
'''{"timestamp": "1456538539.591", "version": "0.1", "txns": [{"request": {"body": "", "timestamp": "1456538539.642", "headers": "GET /dh/ap/social/profile/profile_b32.png HTTP/1.1\r\nHost: s.yimg.com\r\nAccept: */*\r\n\r\n"}, "uuid": "5085d6067ec84876ac8b1949244e3d6b", "response": {"body": "%89PNG%0D%0A%1A", "timestamp": "1456538539.642", "headers": "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nCache-Control: max-age=31536000,public\r\nContent-Length: 564\r\nContent-Type: image/png\r\nDate: Sat, 27 Feb 2016 02:02:16 GMT\r\nEtag: \"YM:1:414a98c1-9991-4029-847c-eda0fc4d115f0004d403c59edc0b\"\r\nExpires: Sat, 05 Sep 2026 00:00:00 GMT\r\nLast-Modified: Thu, 24 Jan 2013 07:29:32 GMT\r\nServer: ATS\r\nVia: HTTP/1.1 web7.use105.mobstor.bf1.yahoo.com UserFiberFramework/1.0, https/1.1 l10.ycs.che.yahoo.com (ApacheTrafficServer [cRs f ])\r\nx-ysws-request-id: 64ec33e0-c032-406b-99a2-415c2941b0d0\r\nx-ysws-visited-replicas: gops.use105.mobstor.vip.bf1.yahoo.com\r\nAge: 3\r\nConnection: keep-alive\r\nY-Trace: BAEAQAAAAABv4pjHA9uFQwAAAAAAAAAA2Mzo_26IdyMAAAAAAAAAAAAFLLbOQZJrAAUsts5Bk.SeWj.KAAAAAA--\r\n\r\n"}}], "encoding": "url_encoded"}
'''
scheme = {'http','https'}
method = {'GET','POST','HEAD'}

def GenTxn():
    global count
    txn = dict()
    txn["request"] = dict()
    txn["request"]["body"]=""
    txn["request"]["timestamp"]="1234"
    txn["request"]["headers"]="GET /{0} HTTP/1.1\r\nHost: s2.yimg.com\r\n\r\n".format(count)
    count = count+1
    txn["uuid"]="123455666"
    
    txn["response"] = dict()
    txn["response"]["body"]=""
    txn["response"]["timestamp"]="1234"
    txn["response"]["headers"]="HTTP/1.1 200 OK\r\nCache-Control: max-age=31536000,public\r\nContent-Length: 564\r\nContent-Type: image/png\r\n\r\n"
    return txn

def GenTxns(nTxn):
    txns = []
    for i in range(0,nTxn):
        txns.append(GenTxn())
    
    return txns

def GenSession():
    session = dict()
    session["timestamp"] = "1234"
    session["version"] = "0.1"
    n = random.randint(1,20)
    session["txns"] = GenTxns(n)
    
    return session

def Generate(n,out_dir):
    for i in range(0,n):
      session = GenSession()
      try:
        sessionfile = open(os.path.join(out_dir, 'session_{0}.json'.format(i)), 'w')
        json.dump(session,sessionfile)
      except:
        print("Error writing to file:{0}:{1}".format(sys.exc_info()[0],sys.exc_info()[1]))
 
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--number", "-n",
                        required=True,
                        type = int,
                        help="Number of sessions to generate"
                        )
                        
    parser.add_argument("--dir", "-d",
                        required=True,
                        help="Directory to store the generated sessions"
                        )
    args = parser.parse_args()
    Generate(args.number,args.dir)

if __name__ == '__main__':
    main()
