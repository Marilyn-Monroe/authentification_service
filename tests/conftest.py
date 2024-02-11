import pathlib
import json

import pytest

from testsuite.databases.pgsql import discover
from testsuite.databases.redis import service

pytest_plugins = [
    'pytest_userver.plugins.postgresql',
    'pytest_userver.plugins.redis'
]


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
        'auth_service',  # service name that goes to the DB connection
        [service_source_dir.joinpath('postgresql/schemas')],
    )
    return pgsql_local_create(list(databases.values()))


@pytest.fixture(scope='session')
def service_env(
        _redis_service_settings: service.ServiceSettings,
):
    secdist_config = {
        'redis_settings': {
            'redis_db_1': {
                'password': '',
                'sentinels': [
                    {
                        'host': 'redis-node-1',
                        'port': 7000
                    },
                    {
                        'host': 'redis-node-2',
                        'port': 7001
                    },
                    {
                        'host': 'redis-node-3',
                        'port': 7002
                    },
                    {
                        'host': 'redis-node-4',
                        'port': 7003
                    },
                    {
                        'host': 'redis-node-5',
                        'port': 7004
                    },
                    {
                        'host': 'redis-node-6',
                        'port': 7005
                    }
                ],
                'shards': [
                    {
                        'name': 'test_master0'
                    },
                    {
                        'name': 'test_master1'
                    },
                    {
                        'name': 'test_master2'
                    }
                ]
            }
        }
    }
    return {'SECDIST_CONFIG': json.dumps(secdist_config)}
