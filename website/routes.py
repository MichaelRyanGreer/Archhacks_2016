from flask import Flask, render_template, url_for, send_from_directory

app = Flask(__name__)

@app.route('/')
def index():
	return render_template('index.html')

#@app.route('/static/js/<path:path>')
#def server_static(path):
#	return send_from_directory('static/js', path)

if __name__ == '__main__':
	app.run()