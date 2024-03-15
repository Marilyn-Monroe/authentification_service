import pathlib
import sys

import pytest

from testsuite.databases.pgsql import discover

import handlers.sessions_management_pb2_grpc as sessions_management_services

USERVER_CONFIG_HOOKS = ['prepare_service_config']

pytest_plugins = [
    'pytest_userver.plugins.grpc',
    'pytest_userver.plugins.postgresql'
]


@pytest.fixture
def grpc_service(pgsql, grpc_channel, service_client):
    return sessions_management_services.SessionsManagementServiceStub(
        grpc_channel)


@pytest.fixture(scope='session')
def mock_grpc_sessions_management_session(grpc_mockserver, create_grpc_mock):
    mock = create_grpc_mock(
        sessions_management_services.SessionsManagementServiceServicer)
    (sessions_management_services
        .add_SessionsManagementServiceServicer_to_server(
            mock.servicer, grpc_mockserver, ))
    return mock


@pytest.fixture
def mock_grpc_server(mock_grpc_sessions_management_session):
    with mock_grpc_sessions_management_session.mock() as mock:
        yield mock


@pytest.fixture(scope='session')
def prepare_service_config(grpc_mockserver_endpoint):
    def patch_config(config, config_vars):
        components = config['components_manager']['components']
        components['sessions-management-client'][
            'endpoint'] = grpc_mockserver_endpoint

    return patch_config


def pytest_configure(config):
    sys.path.append(str(
        pathlib.Path(__file__).parent.parent / 'proto/handlers/'))


@pytest.fixture(scope='session')
def service_source_dir():
    """Path to root directory service."""
    return pathlib.Path(__file__).parent.parent


@pytest.fixture(scope='session')
def initial_data_path(service_source_dir):
    """Path for find files with data"""
    return [
        service_source_dir / 'postgresql/data',
    ]


@pytest.fixture(scope='session')
def pgsql_local(service_source_dir, pgsql_local_create):
    """Create schemas databases for tests"""
    databases = discover.find_schemas(
        'authentification_service',
        # service name that goes to the DB connection
        [service_source_dir.joinpath('postgresql/schemas')],
    )
    return pgsql_local_create(list(databases.values()))
