# coding=utf-8
import os
import sys
import resource
import signal
import time
import argparse
import codecs
from flask import Flask, request, jsonify, render_template
from subprocess import PIPE, Popen, TimeoutExpired

app = Flask(__name__)
args = None
# fix the template conflicts between vue and flask
app.jinja_env.variable_start_string = '[['
app.jinja_env.variable_end_string = ']]'

def set_process_rlimits():
  resource.setrlimit(resource.RLIMIT_DATA,(args.memory, args.memory))
  os.setsid()

@app.route("/")
def hello():
  return render_template("index.html")

@app.route('/run', methods=['POST'])
def run():
  result = {}
  content = request.json['content']
  fname = str(time.time())
  ofile = codecs.open(fname + ".cc", "w", "utf-8")
  ofile.write(content)
  ofile.flush()
  os.fsync(ofile.fileno())
  ofile.close()
  cmd = "g++ -std=c++11 -mfloat-abi=softfp -mfpu=neon " + fname + ".cc -o " + fname
  if not args.neon:
    cmd = "g++ -std=c++11 " + fname + ".cc -o " + fname
  p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE)
  cstdout, cstderr = p.communicate()
  if (not os.path.exists(fname)):
    result['content'] = "\n".join([cstdout.decode('utf-8'), cstderr.decode('utf-8')])
    result['status'] = "fail"
    os.system("rm " + fname + ".cc")
    return jsonify(result)
  cmd = "./" + fname
  p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE, preexec_fn=set_process_rlimits)
  try:
    rstdout, rstderr = p.communicate(timeout=args.timeout)
  except TimeoutExpired:
    p.kill()
    os.killpg(os.getpgid(p.pid), signal.SIGTERM)
    rstdout = b""
    rstderr = b"Process exec time out."
  result['content'] = "\n".join([cstdout.decode('utf-8'), cstderr.decode('utf-8'), rstdout.decode('utf-8'), rstderr.decode('utf-8')])
  result['status'] = "success"
  os.system("rm " + fname + ".cc " + fname)
  return jsonify(result)

@app.route('/example', methods=['POST', 'GET'])
def example():
  name = request.json['name']
  content = codecs.open("codelab/" + name + ".cc", "r", "utf-8").readlines()
  content = "".join(content)
  res = {'status' : 'Success', 'content' : content}
  return jsonify(res)

@app.route('/example_list', methods=['POST', 'GET'])
def example_list():
  res = []
  for line in codecs.open("codelab/codelab.txt", "r", "utf-8").readlines():
    tks = line.strip().split(",")
    res.append({'name' : tks[0], 'text' : tks[1]})
  return jsonify(res)

if __name__ == '__main__':
  # parse arguments
  parser = argparse.ArgumentParser()
  parser.add_argument("-p", "--port", type=int, help="server port number", default=8880)
  parser.add_argument("-n", "--neon", type=int, help="wether to use neon instruments", default=1)
  parser.add_argument("-m", "--memory", type=int, help="max memory use in bytes", default=52428800)
  parser.add_argument("-t", "--timeout", type=int, help="running timeout in seconds", default=20)
  args = parser.parse_args()
  app.run(host='0.0.0.0', port=args.port)

