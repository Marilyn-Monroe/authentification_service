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
    assert response.json() == {
        'id': matching.any_integer
    }


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
    assert response.json() == {
        'code': '400',
        'message': matching.any_string
    }


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
    assert response.json() == {
        'code': '400',
        'message': matching.any_string
    }


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
    assert response.json() == {
        'code': '400',
        'message': matching.any_string
    }


async def test_create_new_user_no_username(service_client):
    response = await service_client.post(
        '/v1/signup',
        json={
            'email': 'testuser@email.com',
            'password': 'testpass'
        },
    )
    assert response.status == 400
    assert response.json() == {
        'code': '400',
        'message': matching.any_string
    }


async def test_create_new_user_no_email(service_client):
    response = await service_client.post(
        '/v1/signup',
        json={
            'username': 'testuser',
            'password': 'testpass'
        },
    )
    assert response.status == 400
    assert response.json() == {
        'code': '400',
        'message': matching.any_string
    }


async def test_create_new_user_no_password(service_client):
    response = await service_client.post(
        '/v1/signup',
        json={
            'username': 'testuser',
            'email': 'testuser@email.com'
        },
    )
    assert response.status == 400
    assert response.json() == {
        'code': '400',
        'message': matching.any_string
    }
