/// Apache License 2.0

#include <boost/numeric/odeint.hpp>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Ephemerides/Analytical.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/ThirdBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::physics::env::obj::Celestial;
using ostk::physics::env::obj::celest::Earth;
using ostk::physics::env::obj::celest::Moon;
using ostk::physics::env::obj::celest::Sun;
using ostk::physics::env::ephem::Analytical;
using ostk::physics::coord::Frame;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::units::Length;
using ostk::physics::units::Derived;
using ostk::physics::units::Time;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;

using ostk::astro::NumericalSolver;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::flight::system::dynamics::ThirdBodyGravity;
using ostk::astro::flight::system::dynamics::CentralBodyGravity;
using ostk::astro::flight::system::dynamics::PositionDerivative;

using namespace boost::numeric::odeint;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

class OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        startStateVector_.resize(6);
        startStateVector_ << 7000000.0, 0.0, 0.0, 0.0, 0.0, 0.0;
    }

    // Current state and instant setup, choose equinox as instant to make geometry simple
    // Earth pulls in the -X direction, Sun pulls in the +X direction, and Moon in the +Y direction
    const Instant startInstant_ = Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC);
    const Shared<Celestial> sphericalMoonSPtr_ = std::make_shared<Celestial>(Moon::Spherical());

    NumericalSolver::StateVector startStateVector_;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity, Constructor)
{
    {
        EXPECT_NO_THROW(ThirdBodyGravity thirdBodyGravity(sphericalMoonSPtr_));
    }

    {
        EXPECT_NO_THROW(ThirdBodyGravity thirdBodyGravity(sphericalMoonSPtr_, "test"));
    }

    {
        const Shared<Celestial> sunSPtr = std::make_shared<Celestial>(Sun::Spherical());
        EXPECT_NO_THROW(ThirdBodyGravity thirdBodyGravity(sunSPtr));
    }

    {
        const Shared<Celestial> moonSPtr = std::make_shared<Celestial>(Moon::Spherical());
        EXPECT_NO_THROW(ThirdBodyGravity thirdBodyGravity(moonSPtr));
    }

    {
        const Earth earth = {
            {398600441500000.0, GravitationalParameterSIUnit},
            Length::Meters(6378137.0),
            0.0,
            0.0,
            0.0,
            std::make_shared<Analytical>(Frame::ITRF()),
            std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Undefined),
            std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
            std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Undefined),
        };

        const String expectedString = "{Gravitational Model} is undefined.";

        // Test the throw and the message that is thrown
        EXPECT_THROW(
            {
                try
                {
                    ThirdBodyGravity thirdBodyGravity(std::make_shared<Celestial>(earth));
                }
                catch (const ostk::core::error::runtime::Undefined& e)
                {
                    EXPECT_EQ(expectedString, e.getMessage());
                    throw;
                }
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        const String expectedString = "Cannot calculate third body acceleration for the Earth yet.";

        // Test the throw and the message that is thrown
        EXPECT_THROW(
            {
                try
                {
                    ThirdBodyGravity thirdBodyGravity(std::make_shared<Celestial>(Earth::Spherical()));
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ(expectedString, e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity, IsDefined)
{
    {
        const ThirdBodyGravity thirdBodyGravity(sphericalMoonSPtr_);

        EXPECT_TRUE(thirdBodyGravity.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity, StreamOperator)
{
    {
        const ThirdBodyGravity thirdBodyGravity(sphericalMoonSPtr_);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << thirdBodyGravity << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity, Print)
{
    {
        const ThirdBodyGravity thirdBodyGravity(sphericalMoonSPtr_);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(thirdBodyGravity.print(std::cout, true));
        EXPECT_NO_THROW(thirdBodyGravity.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity, GetName)
{
    {
        const ThirdBodyGravity thirdBodyGravity(sphericalMoonSPtr_);
        EXPECT_TRUE(thirdBodyGravity.getName() != String::Empty());
    }

    {
        const String name = "test";
        const ThirdBodyGravity thirdBodyGravity(sphericalMoonSPtr_, name);
        EXPECT_TRUE(thirdBodyGravity.getName() == name);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity, GetCelestial)
{
    const Shared<Celestial> moonSPtr = std::make_shared<Celestial>(Moon::Spherical());
    const ThirdBodyGravity thirdBodyGravity(moonSPtr);

    EXPECT_TRUE(thirdBodyGravity.getCelestial() == moonSPtr);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity, ApplyContribution)
{
    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(Moon::Spherical());
    ThirdBodyGravity thirdBodyGravity(earthSPtr);

    NumericalSolver::StateVector dxdt(6);
    dxdt.setZero();
    thirdBodyGravity.applyContribution(startStateVector_, dxdt, startInstant_);

    NumericalSolver::StateVector Moon_ReferencePull(6);
    Moon_ReferencePull << 0.0, 0.0, 0.0, -4.620543790697659e-07, 2.948717888154649e-07, 1.301648617451192e-07;
    EXPECT_TRUE(((Moon_ReferencePull - dxdt).array() < 1e-15).all());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity, OneStepSunOnly)
{
    // Setup dynamics
    const Shared<Celestial> sun = std::make_shared<Celestial>(Sun::Spherical());
    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<ThirdBodyGravity>(sun),
    };

    // Perform 1.0s integration step
    runge_kutta4<NumericalSolver::StateVector> stepper;
    stepper.do_step(Dynamics::GetDynamicalEquations(dynamics, startInstant_), startStateVector_, (0.0), 1.0);

    // Set reference pull values for the Sun
    NumericalSolver::StateVector Sun_ReferencePull(6);
    Sun_ReferencePull << 7.000000000000282e+06, -1.266173652819505e-09, -5.501324277544413e-10, 5.618209329643997e-07,
        -2.532321435973975e-09, -1.100253640019350e-09;

    EXPECT_TRUE(((startStateVector_ - Sun_ReferencePull).array() < 1e-15).all());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity, OneStepMoonOnly)
{
    // Setup dynamics
    const Shared<Celestial> moon = std::make_shared<Celestial>(Moon::Spherical());
    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<ThirdBodyGravity>(moon),
    };

    // Perform 1.0s integration step
    runge_kutta4<NumericalSolver::StateVector> stepper;
    stepper.do_step(Dynamics::GetDynamicalEquations(dynamics, startInstant_), startStateVector_, (0.0), 1.0);

    // Set reference pull values for the Earth
    NumericalSolver::StateVector Moon_ReferencePull(6);
    Moon_ReferencePull << 6.999999999999768e+06, 1.474353635647267e-07, 6.508220913373722e-08, -4.620551958115301e-07,
        2.948701962648114e-07, 1.301641965195380e-07;

    EXPECT_TRUE(((startStateVector_ - Moon_ReferencePull).array() < 1e-15).all());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity, OneStepSunMoonEarth)
{
    const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::Spherical());
    const Shared<Celestial> sun = std::make_shared<Celestial>(Sun::Spherical());
    const Shared<Celestial> moon = std::make_shared<Celestial>(Moon::Spherical());

    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earth),
        std::make_shared<ThirdBodyGravity>(sun),
        std::make_shared<ThirdBodyGravity>(moon),
    };

    // Perform 1.0s integration step
    runge_kutta4<NumericalSolver::StateVector> stepper;
    stepper.do_step(Dynamics::GetDynamicalEquations(dynamics, startInstant_), startStateVector_, (0.0), 1.0);

    // Set reference pull values for the Earth
    NumericalSolver::StateVector Earth_Sun_Moon_ReferencePull(6);
    Earth_Sun_Moon_ReferencePull << 6.999995935640380e+06, 4.700487584518332e-06, 2.137317833766671e-06,
        -8.128720814005144, 9.401159910098908e-06, 4.274716925865539e-06;

    EXPECT_TRUE(((startStateVector_ - Earth_Sun_Moon_ReferencePull).array() < 1e-15).all());
}
