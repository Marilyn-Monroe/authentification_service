import pytest
from testsuite.databases import pgsql
from testsuite.utils import matching


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_login_username(service_client):
    response = await service_client.post(
        '/v1/signin',
        json={
            'login': 'testuser',
            'password': 'testpass'
        },
    )
    assert response.status == 200
    assert response.headers['Set-Cookie'] == matching.RegexString(
        '^sessionid=[0-9a-f]{32,}; '
        'Path=/; '
        'Max-Age=2592000; '
        'Secure; '
        'SameSite=Strict; HttpOnly$')
    assert response.json() == matching.PartialDict({
        'sessionid': matching.any_string
    })


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_login_email(service_client):
    response = await service_client.post(
        '/v1/signin',
        json={
            'login': 'test@email.com',
            'password': 'testpass'
        },
    )
    assert response.status == 200
    assert response.headers['Set-Cookie'] == matching.RegexString(
        '^sessionid=[0-9a-f]{32,}; '
        'Path=/; '
        'Max-Age=2592000; '
        'Secure; '
        'SameSite=Strict; HttpOnly$')
    assert response.json() == matching.PartialDict({
        'sessionid': matching.any_string
    })


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_login_wrong_username(service_client):
    response = await service_client.post(
        '/v1/signin',
        json={
            'login': 'testusernone',
            'password': 'testpass'
        },
    )
    assert response.status == 401
    assert response.json() == matching.PartialDict({
        'code': '401',
        'message': matching.any_string
    })


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_login_wrong_email(service_client):
    response = await service_client.post(
        '/v1/signin',
        json={
            'login': 'testusernone@email.com',
            'password': 'testpass'
        },
    )
    assert response.status == 401
    assert response.json() == matching.PartialDict({
        'code': '401',
        'message': matching.any_string
    })


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_login_wrong_password(service_client):
    response = await service_client.post(
        '/v1/signin',
        json={
            'login': 'test@email.com',
            'password': 'testpassnone'
        },
    )
    assert response.status == 401
    assert response.json() == matching.PartialDict({
        'code': '401',
        'message': matching.any_string
    })


async def test_login_miss_login(service_client):
    response = await service_client.post(
        '/v1/signin',
        json={
            'password': 'testpassnone'
        },
    )
    assert response.status == 400
    assert response.json() == matching.PartialDict({
        'code': '400',
        'message': matching.any_string
    })


async def test_login_miss_password(service_client):
    response = await service_client.post(
        '/v1/signin',
        json={
            'login': 'test@email.com'
        },
    )
    assert response.status == 400
    assert response.json() == matching.PartialDict({
        'code': '400',
        'message': matching.any_string
    })


async def test_login_miss_data(service_client):
    response = await service_client.post(
        '/v1/signin'
    )
    assert response.status == 400
    assert response.json() == matching.PartialDict({
        'code': '400',
        'message': matching.any_string
    })
