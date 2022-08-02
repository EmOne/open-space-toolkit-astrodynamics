////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/SatelliteDynamics.hpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_SatelliteDynamics__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_SatelliteDynamics__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Data/Vector.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{
namespace dynamics
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::core::types::Integer ;
using ostk::core::types::Real ;
using ostk::core::types::String ;
using ostk::core::types::Shared ;
using ostk::core::ctnr::Array ;

using ostk::math::obj::Vector3d ;

using ostk::physics::Environment ;
using ostk::physics::time::Instant ;
using ostk::physics::time::Duration ;
using ostk::physics::coord::Position ;
using ostk::physics::coord::Velocity ;
using ostk::physics::coord::Frame ;
using ostk::physics::data::Vector ;
using ostk::physics::env::obj::celest::Earth ;
using ostk::physics::env::obj::celest::Moon ;
using ostk::physics::env::obj::celest::Sun ;

using ostk::astro::trajectory::State ;
using ostk::astro::flight::system::SatelliteSystem ;
using ostk::astro::flight::system::Dynamics ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Defines a satellite in orbit subject to forces of varying fidelity. Represents a system of differential equations that can be solved by calling the NumericalSolver class.

class SatelliteDynamics : public Dynamics
{

    public:

        /// @brief              Constructor
        ///
        /// @code
        ///                     Environment environment = { ... } ;
        ///                     SatelliteSystem satelliteSystem = { ... } ;
        ///                     State state = { ... } ;
        ///                     SatelliteDynamics satelliteDynamics = { environment, satelliteSystem, state } ;
        /// @endcode
        ///
        /// @param              [in] anEnvironment An environment
        /// @param              [in] aSatelliteSystem A satellite system
        /// @param              [in] aState A 3 DOF state

                                SatelliteDynamics                           (   const   Environment&                anEnvironment,
                                                                                const   SatelliteSystem&            aSatelliteSystem,
                                                                                const   State&                      aState                                      ) ;

        /// @brief              Copy Constructor
        ///
        /// @param              [in] SatelliteDynamics A satellite dynamics

                                SatelliteDynamics                           (   const   SatelliteDynamics&          aSatelliteDynamics                          ) ;

        /// @brief              Destructor

        virtual                 ~SatelliteDynamics                          ( ) override ;

        /// @brief              Clone satellite dynamics
        ///
        /// @return             Pointer to cloned satellite dynamics

        virtual SatelliteDynamics* clone                                    ( ) const override ;

        /// @brief              Equal to operator
        ///
        /// @param              [in] aSatelliteDynamics A satellite dynamics
        /// @return             True if satellite dynamics are equal

        bool                    operator ==                                 (   const   SatelliteDynamics&          aSatelliteDynamics                          ) const ;

        /// @brief              Not equal to operator
        ///
        /// @param              [in] aSatelliteDynamics A satellite dynamics
        /// @return             True if satellite dynamics are not equal

        bool                    operator !=                                 (   const   SatelliteDynamics&          aSatelliteDynamics                          ) const ;

        /// @brief              Output stream operator
        ///
        /// @param              [in] anOutputStream An output stream
        /// @param              [in] aSatelliteDynamics A satellite dynamics
        /// @return             A reference to output stream

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   SatelliteDynamics&          aSatelliteDynamics                          ) ;

        /// @brief              Check if satellite dynamics is defined
        ///
        /// @return             True if satellite dynamics is defined

        virtual bool            isDefined                                   ( ) const override ;

        /// @brief              Print satellite dynamics
        ///
        /// @param              [in] anOutputStream An output stream
        /// @param              [in] (optional) displayDecorators If true, display decorators

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const override ;

        /// @brief              Get satellite dynamics's 3 DOF State
        ///
        /// @code
        ///                     State state = satelliteDynamics.getState() ;
        /// @endcode
        ///
        /// @return             State

        State                   getState                                    ( ) const ;

        /// @brief              Set satellite dynamics's 3 DOF State
        ///
        /// @code
        ///                     State state = { ... } ;
        ///                     satelliteDynamics.setState(state) ;
        /// @endcode
        /// @param              [in] aState A 3DOF state

        void                    setState                                    (   const   State&                      aState                                      ) ;

        /// @brief              Obtain dynamical equations function wrapper
        ///
        /// @code
        ///                     Dynamics::DynamicalEquationWrapper dyneq = satelliteDynamics.getDynamicalEquations() ;
        /// @endcode
        /// @return             std::function<void(const std::vector<double>&, std::vector<double>&, const double)>

        virtual Dynamics::DynamicalEquationWrapper getDynamicalEquations    ( ) override ;

    private:

        Environment             environment_ ;
        Shared<const Frame>     gcrfSPtr_ ;
        SatelliteSystem         satelliteSystem_ ;
        State                   state_ ;

        // Only currently used force model that incorporates only Earth's gravity
        void                    DynamicalEquations                          (   const   Dynamics::StateVector&      x,
                                                                                        Dynamics::StateVector&      dxdt,
                                                                                const   double                      t                                           ) ;

        // // Atmospheric perturbations only
        // void                    Exponential_Dynamics                        (   const   SatelliteDynamics::StateVector&     x,
        //                                                                                 SatelliteDynamics::StateVector&     dxdt,
        //                                                                         const   double                                                                  ) const ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
