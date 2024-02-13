import pytest
from testsuite.databases import pgsql
from testsuite.utils import matching


async def test_create_new_user(service_client):
    response = await service_client.post(
        '/v1/signup',
        json={
            'username': 'testuser',
            'email': 'testuser@email.com',
            'password': 'testpass'
        },
    )
    assert response.status == 201
    assert response.json() == matching.PartialDict({
        'id': matching.any_integer
    })


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_create_existing_username(service_client):
    response = await service_client.post(
        '/v1/signup',
        json={
            'username': 'testuser',
            'email': 'different@email.com',
            'password': 'testpass'
        },
    )
    assert response.status == 400
    assert response.json() == matching.PartialDict({
        'code': '400',
        'message': matching.any_string
    })


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_create_existing_email(service_client):
    response = await service_client.post(
        '/v1/signup',
        json={
            'username': 'differentuser',
            'email': 'test@email.com',
            'password': 'testpass'
        },
    )
    assert response.status == 400
    assert response.json() == matching.PartialDict({
        'code': '400',
        'message': matching.any_string
    })


async def test_create_new_user_wrong_username(service_client):
    response = await service_client.post(
        '/v1/signup',
        json={
            'username': 'wrong',
            'email': 'testuser@email.com',
            'password': 'testpass'
        },
    )
    assert response.status == 400
    assert response.json() == matching.PartialDict({
        'code': '400',
        'message': matching.any_string
    })


async def test_create_new_user_wrong_email(service_client):
    response = await service_client.post(
        '/v1/signup',
        json={
            'username': 'testuser',
            'email': 'wrong',
            'password': 'testpass'
        },
    )
    assert response.status == 400
    assert response.json() == matching.PartialDict({
        'code': '400',
        'message': matching.any_string
    })


async def test_create_new_user_wrong_password(service_client):
    response = await service_client.post(
        '/v1/signup',
        json={
            'username': 'testuser',
            'email': 'testuser@email.com',
            'password': 'wrong'
        },
    )
    assert response.status == 400
    assert response.json() == matching.PartialDict({
        'code': '400',
        'message': matching.any_string
    })


async def test_create_new_user_no_username(service_client):
    response = await service_client.post(
        '/v1/signup',
        json={
            'email': 'testuser@email.com',
            'password': 'testpass'
        },
    )
    assert response.status == 400
    assert response.json() == matching.PartialDict({
        'code': '400',
        'message': matching.any_string
    })


async def test_create_new_user_no_email(service_client):
    response = await service_client.post(
        '/v1/signup',
        json={
            'username': 'testuser',
            'password': 'testpass'
        },
    )
    assert response.status == 400
    assert response.json() == matching.PartialDict({
        'code': '400',
        'message': matching.any_string
    })


async def test_create_new_user_no_password(service_client):
    response = await service_client.post(
        '/v1/signup',
        json={
            'username': 'testuser',
            'email': 'testuser@email.com'
        },
    )
    assert response.status == 400
    assert response.json() == matching.PartialDict({
        'code': '400',
        'message': matching.any_string
    })


async def test_create_new_user_miss_data(service_client):
    response = await service_client.post(
        '/v1/signup'
    )
    assert response.status == 400
    assert response.json() == matching.PartialDict({
        'code': '400',
        'message': matching.any_string
    })
