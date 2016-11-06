from flask import Flask, render_template, request, jsonify
from flask.ext.pymongo import PyMongo
from urllib import request as httprequest
from urllib.parse import urlencode

app = Flask(__name__)
mongo = PyMongo(app)

base_query = "https://api.tropo.com/1.0/sessions?action=create&token=644f69544d416f434350744b4274486f72596b55796f715555684a534d464762737a55727352774e526b5476&"

@app.route('/')
def index():
	return render_template('home.html')

@app.route('/api', methods=['GET', 'POST'])
def api():
	error = None
	if request.method == 'POST':
		#print("New POST Request:")
		#print(request.form)
		patient_id = int(request.form['patient_id'])
		patient = mongo.db.patients.find_one({'_id' : patient_id})
		patient_name = patient['name']
		#print(patient)
		if patient is not None:
			for contact in patient['contacts']:
				query = [('send_to',contact['number']),('patient_name',patient_name)]
				new_request = base_query + urlencode(query=query,encoding='utf-8')
				print(new_request)
				httprequest.urlopen(new_request)
			error = "Success!"
		else:
			error = "User not found"
	else:
		error = "Incorrect usage of API"
	return jsonify(**{"error" : error})

#@app.route('/static/js/<path:path>')
#def server_static(path):
#	return send_from_directory('static/js', path)

if __name__ == '__main__':
	app.run(host='0.0.0.0',port=80)