from flask import Flask, render_template, send_from_directory

import sys, os, signal, json, time, webbrowser, threading, random

import plotly
import pandas as pd
import numpy as np

app = Flask(__name__)
app.debug = True

data = []

@app.route('/')
def index():

    data = []
    stopReason = ""

    # Get process output file and process values.
    with open(app.config.get('outputPath'), 'r') as f:
        # Handle first line with column names.
        lines = f.readlines()

        if len(lines) <= 1:
            return "Given file is empty."

        header = lines.pop(0).split(',')
        # TODO: remove \n from last element

        # TODO: do intelligent grouping of values.
        #       if there are multiple values with the same prefix in a row just group them.
        # TODO: mayby use single plots for each group.

        if len(sys.argv) > 5:
            a = {}
            a['x'] = []
            a['y'] = []
            a['type'] = "lines+markers"
            a['name'] = "value " + sys.argv[5]

            for line in lines:
                # Stop if there is now new line at the moment.
                if line.startswith("->"):
                    stopReason = line
                    break
                if not line:
                    break

                split = line.split(':')
                round = int(split[0])

                state = split[1].split(',')
                for i in range(len(header)):
                    a['x'].append(round)
                    a['y'].append(float(state[int(sys.argv[5])]))

            data.append(a)

        else:
            # Init one array per graph stroke
            for column in header:
                a = {}
                a['x'] = []
                a['y'] = []
                a['type'] = "lines+markers"
                a['name'] = column
                data.append(a)

            for line in lines:
                # Stop if there is now new line at the moment.
                if line.startswith("->"):
                    stopReason = line
                    break
                if not line:
                    break

                split = line.split(':')
                round = int(split[0])

                state = split[1].split(',')
                for i in range(len(header)):
                    data[i]['x'].append(round)
                    data[i]['y'].append(float(state[i]))

        f.close()

    graphs = []
    # Append first graph.
    graphs.append(dict(
                data=data,
                layout=dict(
                    title='Output ({})'.format(stopReason)
                    )
                )
    )

    # Add "ids" to each of the graphs to pass up to the client
    # for templating
    ids = ['Output-{}'.format(i) for i, _ in enumerate(graphs)]
    graphJSON = json.dumps(graphs, cls=plotly.utils.PlotlyJSONEncoder)

    return render_template('index.html',ids=ids,graphJSON=graphJSON)

@app.route('/js/<path:path>')
def send_js(path):
    return send_from_directory('static/js', path)

@app.route('/css/<path:path>')
def send_css(path):
    return send_from_directory('static/css', path)

@app.route('/processInfo')
def send_process_info():
    return render_template('index.html')

@app.route('/processInfoIframe')
def send_process_info_iframe():
    return send_from_directory('../../out/', (os.path.splitext(os.path.split(app.config.get('outputPath'))[1])[0] + '.html'))

@app.route('/favicon.ico')
def send_favicon():
    return send_from_directory('static/images', 'favicon.ico')

if __name__ == '__main__':
    # TODO: validate input..

    pid = str(os.getpid())
    pidfile = sys.argv[3]
    app.config['outputPath'] = sys.argv[4]

    if os.path.isfile(pidfile):
        # kill old process.
        with open(pidfile) as f:
            oldpid = int(f.readline())
            f.close()
            try:
                os.kill(oldpid, signal.SIGTERM)
            except OSError:
                pass

    open(pidfile, 'w').write(pid)

    port = sys.argv[2]
    host = sys.argv[1]
    url = "http://{0}:{1}".format(host,port)
    threading.Timer(2, lambda: webbrowser.open(url) ).start()
    app.run(host=host, port=port, debug=False)
