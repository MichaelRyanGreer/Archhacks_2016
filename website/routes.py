from flask import Flask, render_template, request
from flask.ext.pymongo import PyMongo

app = Flask(__name__)
#mongo = PyMongo(app)
print app.name

@app.route('/')
def index():
	return render_template('home.html')

#@app.route('/api', method='POST')
#def api():
#	error = none



#@app.route('/static/js/<path:path>')
#def server_static(path):
#	return send_from_directory('static/js', path)

if __name__ == '__main__':
	app.run(debug=True)