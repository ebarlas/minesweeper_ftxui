import json
import boto3
import datetime
import re
import base64

client = boto3.client('dynamodb', region_name='us-west-2')


def top_scores(limit):
    res = client.query(
        TableName='minesweeper-marathon-scores',
        ScanIndexForward=False,
        Limit=limit,
        ProjectionExpression='sk',
        KeyConditionExpression='#pk = :pk',
        ExpressionAttributeNames={'#pk': 'pk'},
        ExpressionAttributeValues={':pk': {'N': '1'}})
    return res['Items']


def extract_body(request):
    return json.loads(base64.b64decode(request['body']['data']).decode('utf-8'))


def validate(request):
    body = extract_body(request)
    if 'time' not in body or 'name' not in body or 'score' not in body:
        return False
    time = body['time']
    name = body['name']
    score = body['score']

    if type(time) is not int or type(score) is not int or type(name) is not str:
        return False
    if not re.match(r'[A-Z]{3}', name):
        return False
    if score < 1 or score > 999:
        return False
    if time < 0:
        return False

    return True


def add_score(request):
    time = request['time']
    name = request['name']
    score = request['score']
    date = datetime.datetime.utcfromtimestamp(time).isoformat()
    padding = '0' * (3 - len(str(score)))
    sk = padding + str(score) + '/' + date + '/' + name
    item = {
        'pk': {'N': '1'},
        'sk': {'S': sk}
    }
    client.put_item(TableName='minesweeper-marathon-scores', Item=item)
    print(f'added score, name={name}, score={score}, date={date}')


def convert(items):
    return [{'score': int(a[0]), 'date': a[1], 'name': a[2]} for a in (i['sk']['S'].split('/') for i in items)]


def to_response(items):
    return {
        'status': '200',
        'statusDescription': 'OK',
        'headers': {
            'content-type': [
                {
                    'key': 'Content-Type',
                    'value': 'application/json'
                }
            ]
        },
        'body': json.dumps(items)
    }


def lambda_handler(event, context):
    request = event['Records'][0]['cf']['request']

    print(f'method={request["method"]}, path={request["uri"]}')

    if request['method'] == 'GET' and '/scores' in request['uri']:
        return to_response(convert(top_scores(100)))

    if request['method'] == 'POST' and '/scores' in request['uri']:
        if not validate(request):
            return {'status': '400', 'statusDescription': 'Bad Request'}
        add_score(extract_body(request))
        return {'status': '200', 'statusDescription': 'OK'}

    return {
        'status': '404',
        'statusDescription': 'Not Found',
        'body': 'Not Found'
    }
